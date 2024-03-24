#include <stdio.h>
#include <sys/ptrace.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <sys/types.h>
#include <unistd.h>

#include <sys/user.h>


#define LONGSIZE  sizeof(long)

//  附加到正在运行的进程
int ptrace_attach(pid_t target_pid);
// 让子进程继续运行
int ptrace_cont(pid_t target_pid);
//  结束对目标进程的跟踪
int ptrace_detach(pid_t target_pid);
// 获取寄存器
int get_registers(pid_t target_pid, struct user_regs_struct *regs);
//  恢复寄存器
int set_registers(pid_t target_pid, struct user_regs_struct *regs);

// 读取内存中的数据
int getdata(pid_t target_pid, unsigned long addr, uint8_t *dst, unsigned long len);
// 往内存中写入数据
int putdata(pid_t target_pid, unsigned long addr, uint8_t *src, unsigned long len);
