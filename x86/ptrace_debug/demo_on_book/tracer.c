#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <elf.h>
#include <linux/limits.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/mman.h>

typedef struct handle {
	Elf64_Ehdr *ehdr; 
	Elf64_Phdr *phdr; 
	Elf64_Shdr *shdr; 
	uint8_t *mem;     
	char *symname;    // function name 需要下断点的函数name
	Elf64_Addr symaddr; // function address 需要下断点的函数的地址
	struct user_regs_struct pt_reg;
	char *exec; // executable file address 可执行文件路径
} handle_t;

/**
 * 根据符号名查找符号地址 
 */
Elf64_Addr lookup_symbol(handle_t *, const char *);
// 根据pid获取 进程路径
char * get_pid_path(int pid);
// 信号处理
void sighandler(int);
int global_pid;

#define EXE_MODE 0  // 调试 mode
#define PID_MODE 1  // attach mode

int main(int argc, char **argv, char **envp) {
	int fd, c, mode = 0;
	handle_t h;
	struct stat st;
	long trap, orig;
	int status, pid;
	char * args[2];
	struct user_regs_struct pt_reg2;
	if (argc < 3) {
		printf("Usage: %s [-ep <exe>/<pid>] [-f <fname>]\n", argv[0]);
		exit(0);
	}
	//初始化 handle_t h 
	memset(&h, 0, sizeof(handle_t));

	while ((c = getopt(argc, argv, "p:e:f:")) != -1) {
		switch(c) {
			case 'p': // -p pid
				pid = atoi(optarg);
				h.exec = get_pid_path(pid);
				if (h.exec == NULL) {
					printf("Unable to retrieve executable path for pid: %d\n", pid);
					exit(-1);
				}
				mode = PID_MODE;
				break;
			case 'e': // -e executalue file path
				if ((h.exec = strdup(optarg)) == NULL) {
					perror("strdup");
					exit(-1);
				}
				mode = EXE_MODE;
				break;
			case 'f': // -f function name
				/* 获取指定符号名 */
				if ((h.symname = strdup(optarg)) == NULL) {
					perror("strdup");
					exit(-1);
				}
				break;
			default:
				printf("Unknown option\n");
				break;
		}
	}

	if (h.symname == NULL) {
		printf("Specifying a function name with -f option is required\n");
		exit(-1);
	}

	if (mode == EXE_MODE) {
		args[0] = h.exec;
		args[1] = NULL;
	}
	
	global_pid = pid;
	signal(SIGINT, sighandler);

	/* 打开指定文件 */
	if ((fd = open(h.exec, O_RDONLY)) < 0) {
		perror("open");
		exit(-1);
	}

	/* 获取指定文件的属性 */
	if (fstat(fd, &st) < 0) {
		perror("fstat");
		exit(-1);	
	}

	/* 将目标文件映射到本进程的虚拟内存中 */
	h.mem = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (h.mem == MAP_FAILED) {
		perror("mmap");
		exit(-1);
	}

	h.ehdr = (Elf64_Ehdr *)h.mem;
	h.phdr = (Elf64_Phdr *)(h.mem + h.ehdr->e_phoff);
	h.shdr = (Elf64_Shdr *)(h.mem + h.ehdr->e_shoff);

	/* 确保文件为ELF格式文件, '\0x7FELF' */
	if (h.mem[0] != 0x7f && !strcmp((char *)&h.mem[1], "ELF")) {
		printf("%s is not an ELF file\n",h.exec);
		exit(-1);
	}

	/* 确保目标文件为可执行ELF文件, gcc编译时需要加-no-pie参数 */
	if (h.ehdr->e_type != ET_EXEC) {
		printf("%s is not an ELF executable\n", h.exec);
		exit(-1);
	}

	/* 确保程序在编译过程中没有去掉节表 */
	if (h.ehdr->e_shstrndx == 0 || h.ehdr->e_shoff == 0 || h.ehdr->e_shnum == 0) {
		printf("Section header table not found\n");	
		exit(-1);
	}

	/* 寻找目标符号地址 */
	if ((h.symaddr = lookup_symbol(&h, h.symname)) == 0) {
		printf("Unable to find symbol: %s not found in executable\n", h.symname);
		exit(-1);
	}

	close(fd);

	if (mode == EXE_MODE) { // 若为调试模式
		// fork child process
		if ((pid = fork()) < 0) {
			perror("fork");
			exit(-1);
		}	

		if (pid == 0) {
			// child process 允许被附加
			if (ptrace(PTRACE_TRACEME, pid, NULL, NULL) < 0) {
				perror("PTRACE_TRACEME");
				exit(-1);
			}
			// 用 h.exec image替换 当前进程镜像
			execve(h.exec, args, envp);	
			exit(0);
		}
	} else {
		// 若为 attach 模式，直接附加
		if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) < 0) {
			perror("PTRACE_ATTACH");
			exit(-1);
		}
	}

	/* 等待子进程运行终止，获取子进程状态放入status中 */
	wait(&status);

	printf("Beginning analysis of pid: %d at %lx\n", pid, h.symaddr);
	/* 读取被追踪进程中地址为h.symaddr处的数据 */
	orig = ptrace(PTRACE_PEEKTEXT, pid, h.symaddr, NULL);

	/* int3指令码 0xcc */
	trap = (orig & ~0xff) | 0xcc;

	/* 将int3指令替换掉print_string的第一条指令从而设置断点 */
	if (ptrace(PTRACE_POKETEXT, pid, h.symaddr, trap) < 0) {
		perror("PTRACE_POKETEXT");
		exit(-1);
	}

trace:
	/* 重启已终止的被追踪进程 */
	if (ptrace(PTRACE_CONT, pid, NULL, NULL) < 0) {
		perror("PTRACE_CONT");
		exit(-1);
	} 

	wait(&status);

	/* 由于breakpoint断点、trap终止运行 */
	// 判断进程是否被暂停,及暂停号是否为 SIGTRAP (SIGTRAP 实现相关的硬件异常。一般是调试异常)
	if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
		/* 获取寄存器内容 */
		if (ptrace(PTRACE_GETREGS, pid, NULL, &h.pt_reg) < 0) {
			perror("PTRACE_GETREGS");	
			exit(-1);
		}

		printf("\nExecutable %s (pid: %d) has hit breakpoint 0x%lx\n", h.exec, pid, h.symaddr);

		printf("%%rcx: %llx\n%%rdx: %llx\n%%rbx: %llx\n"
				"%%rax: %llx\n%%rdi: %llx\n%%rsi: %llx\n"
				"%%r8: %llx\n%%r9: %llx\n%%r10: %llx\n"
				"%%r11: %llx\n%%r12 %llx\n%%r13 %llx\n"
				"%%r14: %llx\n%%r15: %llx\n%%rsp: %llx", 
				h.pt_reg.rcx, h.pt_reg.rdx, h.pt_reg.rbx, 
				h.pt_reg.rax, h.pt_reg.rdi, h.pt_reg.rsi,
				h.pt_reg.r8, h.pt_reg.r9, h.pt_reg.r10, 
				h.pt_reg.r11, h.pt_reg.r12, h.pt_reg.r13, 
				h.pt_reg.r14, h.pt_reg.r15, h.pt_reg.rsp);

		printf("\nPlease hit any key to continue: ");
		getchar();

		/* 还原print_string中的第一条指令内容 */
		if (ptrace(PTRACE_POKETEXT, pid, h.symaddr, orig) < 0) {
			perror("PTRACE_POKETEXT");
			exit(-1);
		}

		/* 将指令指针-1，即从print_string的第一条指令开始执行 */
		h.pt_reg.rip = h.pt_reg.rip - 1;

		/* 设置寄存器内容 */
		if (ptrace(PTRACE_SETREGS, pid, NULL, &h.pt_reg) < 0) {
			perror("PTRACE_SETREGS");
			exit(-1);
		}

		/* 设置进程执行一条指令后切换到终止状态 */
		if (ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL) < 0) {
			perror("PTRACE_SINGLESTEP");
			exit(-1);
		}

		wait(NULL);

		/* 继续设置断点 */
		if (ptrace(PTRACE_POKETEXT, pid, h.symaddr, trap) < 0) {
			perror("PTRACE_POKETEXT");
			exit(-1);
		}

		goto trace;
	}

	/* 判断进程是否正常退出 */
	if (WIFEXITED(status)) 
		printf("Completed tracing pid: %d\n", pid);

	exit(0);
}

Elf64_Addr lookup_symbol(handle_t *h, const char *symname) {
	unsigned long i, j, NumOfSym;
	char *strtab;
	Elf64_Sym *symtab;

	for (i = 0; i < h->ehdr->e_shnum; i++) {
		/* 找 .symtab 节，该节保存了符号信息，每一个符号项为Elf64_Sym */
		if (h->shdr[i].sh_type == SHT_SYMTAB) {
			/* 类型为SHT_SYMTAB的节，其sh_link为字符串表所在节表中的下标 */
			/* 因此h->shdr[i].sh_link为字符串表下标 */
			strtab = (char *) (h->mem + h->shdr[h->shdr[i].sh_link].sh_offset);

			/* 此段代码为测试分析，输出字符串表中可视字符 */
			//printf("0x%lx\n", h->shdr[29].sh_offset);
			//for (j = 0; j < h->shdr[29].sh_size; j++) {
			//    if (strtab[j] >= 0x20 && strtab[j] <= 0x7e) {
			//        printf("%c", strtab[j]);
			//    }
			//}
			//puts("");

			/* 获取符号表的首地址 */
			symtab = (Elf64_Sym *) &h->mem[h->shdr[i].sh_offset];

			NumOfSym = h->shdr[i].sh_size / sizeof(Elf64_Sym);

			for (j = 0; j < NumOfSym; j++) {
				//char *name = (char *) &strtab[symtab->st_name];
				//printf("\nsymbol name : %s\n", name);
				/* st_name为符号名在字符串表中的下标 */
				if (!strncmp(&strtab[symtab->st_name], symname, strlen(symname))) {
					/* st_value 为符号的地址 */
					return symtab->st_value;
				}
				symtab++;
			}
		}
	}

	return 0;
}

char * get_pid_path(int pid) {
	//char dir[PATH_MAX] = {0};
	char *dir = (char *) malloc(PATH_MAX * sizeof(char));
	char path[20]={0};

	sprintf(path, "/proc/%d/exe", pid);
	// dir 保存 进程pid 镜像所在的目录(绝对路径)
	int n = readlink(path, dir, PATH_MAX);
	printf("path : %s, dir : %s\n",path,dir);
	if(n < 0) {
		perror("readlink");
		exit(-1);
	}
	return dir;
}

// 当 Ctrl + C 给当前进程发中断信号时，用这个函数处理中断信号。这个涵数的功能是通过 PTRACE_DETACH 结束跟踪目标进程，然后当前进程通过 exit结束
void sighandler(int sig) {
	printf("Caught SIGINT: Detaching from %d\n", global_pid);

	if (ptrace(PTRACE_DETACH, global_pid, NULL, NULL) < 0 && errno) {
		perror("PTRACE_DETACH");
		exit(-1);
	}
	exit(0);
}
