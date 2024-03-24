#include "ptrace_util.h"

//uint8_t trap = 0xcc;
//uint8_t orig;
//
//// 在 vaddr 处 设置断点，并获取这处的原来数据、方便以后还原
//int set_breakpoint(pid_t pid,size_t vaddr) {
//	printf("+ Set breakpoint at %lx\n", vaddr);
//	
//	getdata(pid, vaddr, &orig, 1);
//	
//	putdata(pid, vaddr, &trap, 1);
//
//	return 0;
//}
//
//// 判断是否运行到断点处，如果运行到断点处就获取寄存器信息
//int is_hit_breakpoint(pid_t pid,struct user_regs_struct *regs) {
//	int status;
//	wait(&status);
//	//printf("status:%d\n", status);
//	if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
//		if (regs != NULL) {	
//			get_registers(pid, regs);
//		}
//		return 0;
//	}
//
//	return -1;
//}
//
//// 恢复断点处的代码, 即还原寄存器信息和此位置原来的数据
//int recovery_breakpoint(pid_t pid,struct user_regs_struct regs) {
//	// 运行到断点时，pc 指向断点后一条指令处. 断点指令 0xcc 只有 1 byte 所以 -1 后就是原来断点处地址
//	regs.rip -= 1;
//	
//	set_registers(pid,&regs);
//
//	putdata(pid, regs.rip, &orig, 1);
//
//	return 0;
//}
//
//int call_function(int pid,size_t func_addr,long paramers[],const unsigned int num_param,struct user_regs_struct regs,long *result) {
//	// 1.先把参数保存起来
//	int tmp_num;
//	if (num_param <= 6) {
//		tmp_num = num_param;
//	}else{
//		tmp_num = 6;
//		int len = num_param - 6;
//		long *param_stack = (long *)calloc(len,8);
//		int i,j = 0;
//		for(i = (num_param - 1); i >= 6; i--,j++) {
//			*(param_stack + j) = paramers[i];
//		}
//		for(int k = 0; k < len; k++) {
//			printf("*(param_stack + %d) = %ld \t",k,*(param_stack + k));
//		}
//		puts("");
//		push_stack(pid, &(regs.rsp), param_stack, len);
//	}
//	
//	switch(tmp_num) {
//		case 6:
//			regs.r9 = paramers[5];
//		case 5:
//			regs.r8 = paramers[4];
//		case 4:
//			regs.rcx = paramers[3];
//		case 3:
//			regs.rdx = paramers[2];
//		case 2:
//			regs.rsi = paramers[1];
//		case 1:
//			regs.rdi = paramers[0];
//			break;
//		case 0:
//			{
//				printf("no paramer ...\n" );
//			}
//	}
//
//	//2.把当前指令的下一条指令入栈,函数往上一级返回的时候要用到
//	// rip = rip -1 是为了让函数重新撞击断点，断点指令0xcc 就1 Byte,当上次撞击断点时 pc 已经指下了断点的下一条指令
//	long rip[1] = {regs.rip - 1};
//	push_stack(pid, &(regs.rsp),rip, 1);
//
//	//3. 把pc指向目标函数首地址
//	regs.rip = func_addr;
//	set_registers(pid,&regs);
//
//	/*
//	 * 为了获取 调用的目标函数的返回值(其实这个返回值存放在rax中)，只能让函数继续运行，直到运行结束 再次撞击原来的断点。
//	 这时才是正解的时机获取到 目标函数的返回值 rax
//	 * *
//	ptrace_cont(pid);
//	struct user_regs_struct call_ret_regs;
//	if (is_hit_breakpoint(pid,&call_ret_regs) == -1) {
//		printf("not hit breakpoint...\n");
//		return -1;
//	}
//	*result = call_ret_regs.rax;
//	//printf("call_function return value = %ld\n",*result);
//	
//	return 0;
//}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage:\n\t%s pid\n", argv[0]);
		return -1;
	}
	pid_t target_pid = atoi(argv[1]);

	ptrace_attach(target_pid);
	int status;
	wait(&status);

	size_t func1Addr = 0x401211;
	set_breakpoint(target_pid,func1Addr);

	ptrace_cont(target_pid);

	struct user_regs_struct breakpoint_regs;
	if (is_hit_breakpoint(target_pid,&breakpoint_regs) == -1) {
		printf("not hit breakpoint...\n");
		return -1;
	}
	show_registers(breakpoint_regs);
	getchar();


	size_t func_addr = 0x401273;
	const unsigned int num_param = 10;
	long paramers[num_param];
	paramers[0] = 100;
	paramers[1] = 200;
	paramers[2] = 300;
	paramers[3] = 400;
	paramers[4] = 500;
	paramers[5] = 600;
	paramers[6] = 700;
	paramers[7] = 800;
	paramers[8] = 900;
	paramers[9] = 901;
	long result;
	call_function(target_pid,func_addr,paramers,num_param,breakpoint_regs,&result);
	printf("call_function return value = %ld\n",result);
	

	recovery_breakpoint(target_pid, breakpoint_regs);

	ptrace_cont(target_pid);

	return 0;
}
