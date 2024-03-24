#include "ptrace_util.h"

//  附加到正在运行的进程
int ptrace_attach(pid_t target_pid) {
	printf("+ Attach process %d\n", target_pid);
	if (ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) < 0) {
		perror("ptrace(ATTACH)");
		return -1;
	}
	printf("+ Waiting for process...\n");
	return 0;
}

// 让子进程继续运行
int ptrace_cont(pid_t target_pid) {
	printf("+ Continue process %d\n", target_pid);
	if (ptrace(PTRACE_CONT, target_pid, NULL, NULL) < 0) {
		perror("ptrace(PTRACE_CONT)");
		return -1;
	}
	return 0;
}

//  结束对目标进程的跟踪
int ptrace_detach(pid_t target_pid) {
	printf("+ Detach process %d\n", target_pid);
	if (ptrace(PTRACE_DETACH, target_pid, NULL, NULL) < 0) {
		perror("ptrace(DETACH)");
		return -1;
	}
	return 0;
}


// 获取寄存器
int get_registers(pid_t target_pid, struct user_regs_struct *regs) {
	printf("+ Getting Registers\n");
	if (ptrace(PTRACE_GETREGS, target_pid, NULL, regs) < 0) {
		perror("ptrace(GETREGS)");
		return -1;
	}
	return 0;
}

//  恢复寄存器
int set_registers(pid_t target_pid, struct user_regs_struct *regs) {
	printf("+ Setting Registers\n");
	if (ptrace(PTRACE_SETREGS, target_pid, NULL, regs)) {
		perror("ptrace(SETREGS)");
		return -1;
	}
	return 0;
}


int getdata(pid_t target_pid, unsigned long addr, uint8_t *dst, unsigned long len) {
	union {
		long val;
		uint8_t bytes[LONGSIZE];
	} data;
	unsigned long i = 0;
	unsigned long j = len / LONGSIZE;
	uint8_t *laddr = dst;
	while (i < j) {
		data.val = ptrace(PTRACE_PEEKTEXT, target_pid, addr + (i * LONGSIZE), NULL);
		memcpy(laddr, data.bytes, LONGSIZE);
		++i;
		laddr += LONGSIZE;
	}

	unsigned long remainder = len % LONGSIZE;
	if (remainder != 0) { // save the remainder, which less than LONGSIZE
		data.val = ptrace(PTRACE_PEEKTEXT, target_pid, addr + (i * LONGSIZE), NULL);
		memcpy(laddr, data.bytes, remainder);
	}

	return 0;
}

int putdata(pid_t target_pid, unsigned long addr, uint8_t *src, unsigned long len) {
	union {
		long val;
		uint8_t bytes[LONGSIZE];
	} data;
	unsigned long i = 0;
	unsigned long j = len / LONGSIZE;
	uint8_t *laddr = src;
	while (i < j) {
		memcpy(data.bytes, laddr, LONGSIZE);
		ptrace(PTRACE_POKETEXT, target_pid, addr + (i * LONGSIZE), data.val);
		++i;
		laddr += LONGSIZE;
	}

	unsigned long remainder = len % LONGSIZE;
	if (remainder != 0) {
		data.val = ptrace(PTRACE_PEEKTEXT, target_pid, addr + (i * LONGSIZE), NULL);
		memcpy(data.bytes, laddr, remainder);
		ptrace(PTRACE_POKEDATA, target_pid, addr + (i * LONGSIZE), data.val);
	}

	return 0;
}

