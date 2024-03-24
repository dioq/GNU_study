#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sys/user.h>

// 让子进程继续运行
void continue_process(pid_t target_pid) {
	if ((ptrace(PTRACE_CONT, target_pid, NULL, NULL)) < 0) {
		perror("ptrace(PTRACE_CONT):");
		exit(1);
	}
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

// 获取寄存器
int get_registers(pid_t target_pid, struct user_regs_struct *regs) {
	printf("+ Getting Registers\n");
 	long ret = ptrace(PTRACE_GETREGS, target_pid, NULL, regs);
	printf("%s: ret = %ld\n",__FUNCTION__ ,ret);
	if (ret < 0) {
		perror("ptrace(GETREGS):");
		exit(-1);
	}
//	(*regs).rip += 2;
	return 0;
}

//  恢复寄存器
void set_registers(pid_t target_pid, struct user_regs_struct *regs) {
	printf("+ Setting instruction pointer to %p\n", (void *) regs->rip);
	long ret = ptrace(PTRACE_SETREGS, target_pid, NULL, regs);
	printf("%s: ret = %ld\n",__FUNCTION__ ,ret);
	if (ret < 0) {
		perror("ptrace(GETREGS):");
		exit(1);
	}
	printf("+ Run it!\n");
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage:\n\t%s pid\n", argv[0]);
		exit(1);
	}

	pid_t target_pid = atoi(argv[1]);
	struct user_regs_struct regs;
	memset((void *)&regs,'\0',sizeof(struct user_regs_struct));

	attach_process(target_pid);
	get_registers(target_pid, &regs);
	printf("regs.rdi [%llx], regs.rsi[%llx], regs.rdx[%llx], regs.rax[%llx], regs.orig_rax[%llx]\n",
			regs.rdi, regs.rsi, regs.rdx,regs.rax, regs.orig_rax);

	set_registers(target_pid, &regs);

	//	while(1) {
	//		printf("%s\n", "this is a test string.");
	//		sleep(3);
	//	}

	continue_process(target_pid);

	return 0;
}
