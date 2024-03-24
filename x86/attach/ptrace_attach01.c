#include <stdio.h>
#include <stdlib.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s <pid to be traced>\n",argv[0]);
		exit(1);
	}
	pid_t traced_pid = atoi(argv[1]);
	ptrace(PTRACE_ATTACH, traced_pid, NULL, NULL);
	wait(NULL);

	struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS, traced_pid, NULL, &regs);

	long ins = ptrace(PTRACE_PEEKTEXT, traced_pid, regs.rip, NULL);
	printf("RIP: 0x%llx Instruction executed: %lx\n",regs.rip, ins);

	//	while(1) {
	//		sleep(3);
	//	}
	ptrace(PTRACE_DETACH, traced_pid, NULL, NULL);

	return 0;
}
