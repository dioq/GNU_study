#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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


int main(int argc, char *argv[]) {   
	if(argc != 2) {
		printf("Usage: %s <pid to be traced>\n", argv[0]);
		exit(1);
	}
	pid_t traced_process = atoi(argv[1]);
	struct user_regs_struct regs;
	long ins;
	/* int 0x80, int3 */
	uint8_t code[] = {0xcd,0x80,0xcc,0};
	uint8_t backup[8];
	
	ptrace(PTRACE_ATTACH, traced_process, NULL, NULL);
	wait(NULL);
	ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);
	
	/* Copy instructions into a backup variable */
	getdata(traced_process, regs.rip, backup, sizeof(backup));
	/* Put the breakpoint */
	putdata(traced_process, regs.rip, code, sizeof(code));
	/* Let the process continue and execute
	   the int 3 instruction */
	ptrace(PTRACE_CONT, traced_process, NULL, NULL);
	wait(NULL);
	
	printf("The process stopped, putting back "
			"the original instructions\n");
	printf("Press <enter> to continue\n");
	getchar();
	
	putdata(traced_process, regs.rip, backup, sizeof(backup));
	/* Setting the eip back to the original
	   instruction to let the process continue */
	ptrace(PTRACE_SETREGS, traced_process, NULL, &regs);
	ptrace(PTRACE_DETACH, traced_process, NULL, NULL);

	return 0;
}
