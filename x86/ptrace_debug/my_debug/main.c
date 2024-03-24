#include "ptrace_util.h"

uint8_t trap = 0xcc;

// 在 vaddr 处 设置断点，并获取这处的原来数据、方便以后还原
int set_breakpoint(pid_t pid,size_t vaddr, uint8_t *orig) {
	printf("+ Set breakpoint at %lx\n", vaddr);
	
	getdata(pid, vaddr, orig, 1);
	
	putdata(pid, vaddr, &trap, 1);

	return 0;
}

// 判断是否运行到断点处，如果运行到断点处就获取寄存器信息
int is_hit_breakpoint(pid_t pid,struct user_regs_struct *regs) {
	int status;
	wait(&status);
        //printf("status:%d\n", status);
        if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
		get_registers(pid, regs);
		return 0;
	}

	return -1;
}

// 恢复断点处的代码, 即还原寄存器信息和此位置原来的数据
int recovery_breakpoint(pid_t pid,struct user_regs_struct *regs,uint8_t *orig) {
	// 运行到断点时，pc 指向断点后一条指令处. 断点指令 0xcc 只有 1 byte 所以 -1 后就是原来断点处地址
	regs->rip -= 1;
	
	set_registers(pid,regs);

	putdata(pid, regs->rip, orig, 1);

	return 0;
}

// 打印寄存器信息
int show_registers(struct user_regs_struct *regs){
	printf("%%rip: %llx, %%rsp: %llx, %%rbp: %llx, %%rax: %llx\n%%rdi: %llx, %%rsi: %llx, %%rdx: %llx, %%rcx: %llx, %%r8: %llx, %%r9: %llx\n%%r10: %llx, %%r11: %llx, %%r12: %llx, %%r13: %llx, %%14: %llx, %%15: %llx\n",
			regs->rip,regs->rsp,regs->rbp,regs->rax,regs->rdi,regs->rsi,regs->rdx,regs->rcx,regs->r8,regs->r9,regs->r10,regs->r11,regs->r12,regs->r13,regs->r14,regs->r15);
	return 0;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage:\n\t%s pid\n", argv[0]);
		return -1;
	}
	pid_t target_pid = atoi(argv[1]);

	ptrace_attach(target_pid);
	int status;
	wait(&status);

	size_t func1Addr = 0x401216;
	uint8_t orig = 0;
	set_breakpoint(target_pid,func1Addr, &orig);

	ptrace_cont(target_pid);

	struct user_regs_struct breakpoint_regs;
	int ret = is_hit_breakpoint(target_pid,&breakpoint_regs);
	if (ret == -1) {
		printf("not hit breakpoint...\n");
		return -1;
	}
	show_registers(&breakpoint_regs);
	getchar();
	recovery_breakpoint(target_pid, &breakpoint_regs, &orig);

	ptrace_cont(target_pid);

	return 0;
}
