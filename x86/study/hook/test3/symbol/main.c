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
char *get_pid_path(int pid);

// 信号处理
void sighandler(int);

int global_pid;

int main(int argc, char **argv) {
    int fd = 0;
    handle_t h;
    struct stat st;
    int status, pid;

/*    if (argc < 2) {
        printf("Usage: %s [-ep <exe>/<pid>] [-f <fname>]\n", argv[0]);
        exit(0);
    }*/
    //初始化 handle_t h
    memset(&h, 0, sizeof(handle_t));	

    pid = atoi(argv[1]);
    h.exec = get_pid_path(pid);
    if (h.exec == NULL) {
        printf("Unable to retrieve executable path for pid: %d\n", pid);
        exit(-1);
    }

    h.symname = "func11";
//    if ((h.symname = argv[2]) == NULL) {
//        perror("strdup");
//        exit(-1);
//    }

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

    h.ehdr = (Elf64_Ehdr *) h.mem;
    h.phdr = (Elf64_Phdr *) (h.mem + h.ehdr->e_phoff);
    h.shdr = (Elf64_Shdr *) (h.mem + h.ehdr->e_shoff);

    /* 确保文件为ELF格式文件, '\0x7FELF' */
    if (h.mem[0] != 0x7f && !strcmp((char *) &h.mem[1], "ELF")) {
        printf("%s is not an ELF file\n", h.exec);
        exit(-1);
    }

    /* 确保目标文件为可执行ELF文件, gcc编译时需要加-no-pie参数 */
//    if (h.ehdr->e_type != ET_EXEC) {
//        printf("%s is not an ELF executable\n", h.exec);
//        exit(-1);
//    }

    /* 确保程序在编译过程中没有去掉节表 */
//    if (h.ehdr->e_shstrndx == 0 || h.ehdr->e_shoff == 0 || h.ehdr->e_shnum == 0) {
//        printf("Section header table not found\n");
//        exit(-1);
//    }

    /* 寻找目标符号地址 */
    if ((h.symaddr = lookup_symbol(&h, h.symname)) == 0) {
        printf("Unable to find symbol: %s not found in executable\n", h.symname);
        exit(-1);
    }

    close(fd);

    /* 重启已终止的被追踪进程 */
    if (ptrace(PTRACE_CONT, pid, NULL, NULL) < 0) {
        perror("PTRACE_CONT");
        exit(-1);
    }

    wait(&status);

    return 0;
}

Elf64_Addr lookup_symbol(handle_t *h, const char *symname) {
    int i, j;
    char *strtab;
    Elf64_Sym *symtab;

    for (i = 0; i < h->ehdr->e_shnum; i++) {
        /* 找 ./symtab 节，该节保存了符号信息，每一个符号项为Elf64_Sym */
        if (h->shdr[i].sh_type == SHT_SYMTAB) {
            /* 类型为SHT_SYMTAB的节，其sh_link为字符串表所在节表中的下标 */
            /* 因此h->shdr[i].sh_link为字符串表下标 */
            strtab = (char *) (h->mem + h->shdr[h->shdr[i].sh_link].sh_offset);

            /* 获取符号表的首地址 */
            symtab = (Elf64_Sym *) &h->mem[h->shdr[i].sh_offset];

            unsigned long NumOfSym = h->shdr[i].sh_size / sizeof(Elf64_Sym);

            for (j = 0; j < NumOfSym; j++) {
                char *name = (char *) &strtab[symtab->st_name];
                printf("\nsymbol name : %s addr : %p\n\n", name, (void *) symtab->st_value);
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

char *get_pid_path(int pid) {
    //char dir[PATH_MAX] = {0};
    char *dir = (char *) malloc(PATH_MAX * sizeof(char));
    char path[20] = {0};

    sprintf(path, "/proc/%d/exe", pid);
    // dir 保存 进程pid 镜像所在的目录(绝对路径)
    int n = readlink(path, dir, PATH_MAX);
    printf("path : %s, dir : %s\n", path, dir);
    if (n < 0) {
        perror("readlink");
        exit(-1);
    }
    return dir;
}

void sighandler(int sig) {
    printf("Caught SIGINT: Detaching from %d\n", global_pid);

    if (ptrace(PTRACE_DETACH, global_pid, NULL, NULL) < 0 && errno) {
        perror("PTRACE_DETACH");
        exit(-1);
    }
    exit(0);
}

