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

int status;

// 5. 目标进程继续运行
void continue_process(pid_t target_pid) {
	if ((ptrace(PTRACE_CONT, target_pid, NULL, NULL)) < 0) {
		perror("ptrace(DETACH):");
		exit(1);
	}
}


// 3. 内存信息
int write_data(pid_t pid, uint8_t *src, void *dst, unsigned long len) {
	unsigned long *s = (unsigned long *) src;
	unsigned long *d = (unsigned long *) dst;

	unsigned long step = sizeof(unsigned long);
	for (unsigned long i = 0; i < len; i += step, s++, d++) {
		if ((ptrace(PTRACE_POKETEXT, pid, d, *s)) < 0) {
			perror("ptrace(POKETEXT):");
			return -1;
		}
	}
	puts("inject success!");
	return 0;
}

// 1. 附加到正在运行的进程
int attach_process(pid_t target_pid) {
	printf("+ Tracing process %d\n", target_pid);
	if ((ptrace(PTRACE_ATTACH, target_pid, NULL, NULL)) < 0) {
		perror("ptrace(ATTACH):");
		exit(-1);
	}
	printf("+ Waiting for process...\n");
	wait(&status);

	return 0;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage:\n\t%s pid\n", argv[0]);
		exit(1);
	}
	pid_t target_pid = atoi(argv[1]);
	printf("%s %d\n", __FUNCTION__, __LINE__);
	attach_process(target_pid);
	printf("%s %d\n", __FUNCTION__, __LINE__);
	long trap, orig;
	struct user_regs_struct pt_reg;
	Elf64_Addr p1 = 0x401d35;
	unsigned long p2 = 0x401d4c;

	printf("%s %d\n", __FUNCTION__, __LINE__);
	// unsigned long *data = (unsigned long *) malloc(sizeof(unsigned long *));
	orig = ptrace(PTRACE_PEEKTEXT, target_pid, p1, NULL);
	wait(&status);

	printf("%s %d\n", __FUNCTION__, __LINE__);
	printf("orig = %lx\n", orig);

	trap = (orig & ~0xff) | 0xcc;
	//    uint8_t p2[] = "000000000401d4c";
	//    unsigned long len = sizeof(p2);
	//    printf("len = %lu", len);
	//    write_data(target_pid, p2, (void *) p1, len);
	if ((ptrace(PTRACE_POKETEXT, target_pid, p1, trap)) < 0) {
		perror("ptrace(POKETEXT):");
		return -1;
	}
	wait(&status);

	printf("%d %s\n", __LINE__, __FUNCTION__);

	if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
		/* 获取寄存器内容 */
		if (ptrace(PTRACE_GETREGS, target_pid, NULL, &pt_reg) < 0) {
			perror("PTRACE_GETREGS");
			exit(-1);
		}
		printf("\nPlease hit any key to continue: ");
		getchar();

		/* 还原print_string中的第一条指令内容 */
		if (ptrace(PTRACE_POKETEXT, target_pid, p1, orig) < 0) {
			perror("PTRACE_POKETEXT");
			exit(-1);
		}

		/* 将指令指针-1，即从print_string的第一条指令开始执行 */
		pt_reg.rip = pt_reg.rip - 1;

		/* 设置寄存器内容 */
		if (ptrace(PTRACE_SETREGS, target_pid, NULL, pt_reg) < 0) {
			perror("PTRACE_SETREGS");
			exit(-1);
		}

		/* 设置进程执行一条指令后切换到终止状态 */
		if (ptrace(PTRACE_SINGLESTEP, target_pid, NULL, NULL) < 0) {
			perror("PTRACE_SINGLESTEP");
			exit(-1);
		}

		wait(NULL);

		/* 继续设置断点 */
		if (ptrace(PTRACE_POKETEXT, target_pid, p1, trap) < 0) {
			perror("PTRACE_POKETEXT");
			exit(-1);
		}
	}

	getchar();
	continue_process(target_pid);
	return 0;
}
