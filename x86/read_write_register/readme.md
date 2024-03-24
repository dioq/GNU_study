X86-64有16个64位寄存器，分别是：
%rax，%rbx，%rcx，%rdx，%esi，%edi，%rbp，%rsp，%r8，%r9，%r10，%r11，%r12，%r13，%r14，%r15
其中：
%rip  即将执行的下一条指令
%rax  临时寄存器或者返回值
%rsp  栈指针寄存器，指向栈顶
%rbp  栈指针寄存器, 指向栈帧
%rdi，%rsi，%rdx，%rcx，%r8, %r9 用作函数参数，依次对应第1参数，第2参数。。。
%rbx，%r12，%r13，%14，%15 用作数据存储，遵循被调用者使用规则，简单说就是随便用，调用子函数之前要备份它，以防他被修改
%r10，%r11 用作数据存储，遵循调用者使用规则，简单说就是使用之前要先保存原值

#include <sys/user.h>
struct user_regs_struct regs;
printf("%%rcx: %llx\n%%rdx: %llx\n%%rbx: %llx\n"
                                "%%rax: %llx\n%%rdi: %llx\n%%rsi: %llx\n"
                                "%%r8:  %llx\n%%r9:  %llx\n%%r10: %llx\n"
                                "%%r11: %llx\n%%r12  %llx\n%%r13  %llx\n"
                                "%%r14: %llx\n%%r15: %llx\n%%rsp: %llx\n"
                                "%%rbp: %llx\n%%rip: %llx\n",
                                regs.rcx, regs.rdx, regs.rbx,
                                regs.rax, regs.rdi, regs.rsi,
                                regs.r8,  regs.r9,  regs.r10,
                                regs.r11, regs.r12, regs.r13,
                                regs.r14, regs.r15, regs.rsp, 
                                regs.rbp, regs.rip);

statck
栈一般从高地址往低地址生长，每个函数都在栈空间中对应一个栈帧。关于栈帧有两个重要的指针——栈基址指针BP和栈顶指针SP。其中，除了在函数的开头和结尾（后面会讲到），BP指针一般是固定不变的，通常以它为基准来寻址参数和局部变量，例如将第一个参数放入栈中的汇编代码可以是movq %rdi, -8(%rbp)。
和栈相关的两个指令是push和pop。在x86_64架构的计算机上，push operand指令的作用是：
将栈顶指针rsp减去8（即8字节）
然后将目标操作数放入更新后的rsp所指向的地址处
pop指令的作用正好和push指令相反：
先将当前rsp指向的地址处的数弹出
然后将rsp增加8
注意：虽然在x86_64机器上也可以push/pop长度小于8字节的操作数，但是一般并不推荐这样做。
