#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <asm/ptrace-abi.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
const int long_size = sizeof(long);

/*主要通过PTRACE_PEEKDATA获取内存中的内容*/
void getdata(pid_t child, long addr, char *str, int len)
{   
    char *backup;
    int i, j;
    union u{
            long val;
            char chars[long_size];
    }data;

    i = 0;
    j = len/long_size;
    backup = str;

    while(i < j) {
        data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 4, NULL);
        memcpy(backup,data.chars,long_size);
        i++;
        backup += long_size;
    }
    j = len % long_size;
    if(j != 0) {
        data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 4, NULL);
        memcpy(backup, data.chars, j);
    }
}

/*与getdata相反，主要通过PTRACE_POKEDATA向内存写内容*/
void putdata(pid_t child, long addr, char *str, int len)
{   
    char *code;
    int i, j;
    union u{
            long val;
            char chars[long_size];
     }data;
    i = 0;
    j = len / long_size;
    code = str;
    while(i < j) {
        memcpy(data.chars, code, long_size);
        ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);/*函数写入是以words为单位的，所以我们我们需要转换成word类型，还需要指针每次增加4。*/
        ++i;
        code += long_size;
    }
    j = len % long_size;
    if(j != 0) {
        memcpy(data.chars, code, j);
        ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);
    }
}

int main(int argc, char *argv[])
{   
    pid_t traced_process;//实际就是int类型
    struct user_regs_struct regs, newregs;//定义数据寄存器数据结构的两个变量，regs，newregs
    long ins;
    /*
    struct user_regs_struct {
    unsigned long    r15;
    unsigned long    r14;
    unsigned long    r13;
    unsigned long    r12;
    unsigned long    bp;
    unsigned long    bx;
    unsigned long    r11;
    unsigned long    r10;
    unsigned long    r9;
    unsigned long    r8;
    unsigned long    ax;
    unsigned long    cx;
    unsigned long    dx;
    unsigned long    si;
    unsigned long    di;
    unsigned long    orig_ax;
    unsigned long    ip;
    unsigned long    cs;
    unsigned long    flags;
    unsigned long    sp;
    unsigned long    ss;
    unsigned long    fs_base;
    unsigned long    gs_base;
    unsigned long    ds;
    unsigned long    es;
    unsigned long    fs;
    unsigned long    gs;
};*/
    /* int 0x80, int 3 */
    char code[] = {0xcd,0x80,0xcc,0}; //定义字符数组，存的是将要插入的机器码
    char backup[4]; //定义接收原内存机器码的字符数组，这里应该和code[]字符数量相对应
    traced_process = atoi(argv[1]); //这里把传入的pid转换成int类型

    /*attack指定pid进程，traced_process*/
    ptrace(PTRACE_ATTACH, traced_process,NULL, NULL);//跟踪pid进程
    wait(NULL); //等待系统通知
    ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);/*获取目标进程的所有寄存器值，存入regs结构体变量中，为以后恢复原rip,以及各个寄存器的值做准备*/
    printf("eip=%lld\n",regs.rip);
    getdata(traced_process, regs.rip, backup, 3);/* 将rip指向地址中的机器码备份到backup中*/
    putdata(traced_process, regs.rip, code, 3); /* 将int 0x80, int 3指令的机器码写入rip指向内存地址中，int 0x80长度为2，int3长度为1*/
    
    /* 让目标进程继续执行并执行我们插入的int 0x80，int 3指令 */
    ptrace(PTRACE_CONT, traced_process, NULL, NULL);
    wait(NULL);//等待系统通知
    printf("This process is attacked by 0xAXSDD! Press <enter> to continue!");
    getchar();//捕获一个<enter>输入
    putdata(traced_process, regs.rip, backup, 3); /*将backup原指令机器码恢复到原rip指向的地址中*/
    ptrace(PTRACE_SETREGS, traced_process, NULL, &regs); /* 让rip指向的内存地址恢复到原本指向的地址，让目标进程继续执行，恢复rip指针 */
    ptrace(PTRACE_DETACH, traced_process, NULL, NULL);/* 结束跟踪*/
    return 0;

}
