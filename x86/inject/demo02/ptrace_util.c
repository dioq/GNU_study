#include "ptrace_util.h"

//  附加到正在运行的进程
int ptrace_attach(pid_t target_pid) {
	printf("+ Attach process %d\n", target_pid);
	if (ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) < 0) {
		perror("ptrace(ATTACH)");
		return -1;
	}
	printf("+ Waiting for process...\n");
	
	int status;
	wait(&status);
	//printf("status:%d\n", status);
	if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGCONT) {
		printf("+ Child process is stopped, by ptrace(PTRACE_ATTACH,...)\n");
		return 0;
	}

	return -1;
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
	printf("+ Getting registers\n");
	long ret = ptrace(PTRACE_GETREGS, target_pid, NULL, regs);
	//printf("%s: ret = %ld\n",__FUNCTION__ ,ret);
	if (ret < 0) {
		perror("ptrace(GETREGS)");
		return -1;
	}
	return 0;
}

// 写入寄存器
int set_registers(pid_t target_pid, struct user_regs_struct *regs) {
	printf("+ Setting registers\n");
	long ret = ptrace(PTRACE_SETREGS, target_pid, NULL, regs);
	//printf("%s: ret = %ld\n",__FUNCTION__ ,ret);
	if (ret < 0) {
		perror("ptrace(SETREGS)");
		return -1;
	}
	//printf("+ Run it!\n");
	return 0;
}

// 打印寄存器信息
int show_registers(struct user_regs_struct regs) {
	printf("%%rip: %llx, %%rsp: %llx, %%rbp: %llx, %%rax: %llx\n%%rdi: %llx, %%rsi: %llx, %%rdx: %llx, %%rcx: %llx, %%r8: %llx, %%r9: %llx\n%%r10: %llx, %%r11: %llx, %%r12: %llx, %%r13: %llx, %%14: %llx, %%15: %llx\n",
			regs.rip,regs.rsp,regs.rbp,regs.rax,regs.rdi,regs.rsi,regs.rdx,regs.rcx,regs.r8,regs.r9,regs.r10,regs.r11,regs.r12,regs.r13,regs.r14,regs.r15);
	return 0;
}

// 打印此时的寄存器信息
int show_current_registers(pid_t pid) {
	struct user_regs_struct regs;
	get_registers(pid, &regs);
	show_registers(regs);
	return 0;
}


int getdata(pid_t target_pid, unsigned long addr, uint8_t *dst, unsigned long len) {
	printf("+ Peektext process %d\n", target_pid);
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
	printf("+ Poketext process %d\n", target_pid);
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
		ptrace(PTRACE_POKETEXT, target_pid, addr + (i * LONGSIZE), data.val);
	}

	return 0;
}

// 以地址 sp 为基址，往这块内堆中 push 数据，可以操纵远程进程 stack
int push_stack(int pid, unsigned long long *sp, long * paramers,int len) {
	const unsigned int MAX_STACK = 80000;
	union {
		long val[MAX_STACK];
		uint8_t bytes[MAX_STACK * LONGSIZE];
	} data;

	unsigned long statck_delta = len * LONGSIZE;
	*sp -= statck_delta;

	for (unsigned int i = 0; i < len; i++) {
		data.val[len - 1 - i] = paramers[i];
	}

	putdata(pid, *sp, data.bytes, statck_delta);

	return 0;
}

// 以地址 sp 为基址，往这块内堆中 pop  数据，可以操纵远程进程 stack
int pop_stack(int pid, unsigned long long *sp, long *paramers, int len) {
	const unsigned int MAX_STACK = 80000;
	union {
		long val[MAX_STACK];
		uint8_t bytes[MAX_STACK * LONGSIZE];
	} data;

	unsigned long statck_delta = len * LONGSIZE;

	//printf("pop new *sp = 0x%lx\n", *sp);
	getdata(pid, *sp, data.bytes, statck_delta);
	for (int i = 0; i < len; i++) {
		paramers[i] = data.val[i];
	}

	*sp += statck_delta;

	return 0;
}

// 断点指令
uint8_t trap = 0xcc;
// 在 vaddr 处 设置断点，并获取这处的原来数据、方便以后还原
int set_breakpoint(pid_t pid,size_t vaddr) {
	printf("+ Set breakpoint at %lx\n", vaddr);

	getdata(pid, vaddr, &orig, 1);

	putdata(pid, vaddr, &trap, 1);

	return 0;
}

// 判断是否运行到断点处，如果运行到断点处就获取寄存器信息
int wait_breakpoint(pid_t pid) {
	int status;
	waitpid(pid, &status, WUNTRACED);
	//printf("status:%d\n", status);
	if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
		printf("+ Child process is stopped, by breakpoint ...\n");
		return 0;
	}

	return -1;
}

// 恢复断点处的代码并使程序继续运行, 即还原寄存器信息和此位置原来的数据 然后调用 ptrace(PTRACE_CONT,...)
int recovery_breakpoint(pid_t pid,struct user_regs_struct regs) {
	// 先运行到断点处
//	wait_breakpoint();

	// 运行到断点时，pc 指向断点后一条指令处. 断点指令 0xcc 只有 1 byte 所以 -1 后就是原来断点处地址
	regs.rip -= 1;
	
	// 恢复寄存器数据
	set_registers(pid,&regs);

	// 清除原来设置的断点，也就是把之前写入 0xcc 处的原始数据再写回去
	putdata(pid, regs.rip, &orig, 1);

	ptrace_cont(pid);

	return 0;
}

// 远程调用函数
int call_function(pid_t pid,size_t func_addr,long paramers[], uint8_t num_param,long long *result) {
	struct user_regs_struct regs;
	get_registers(pid, &regs);
	// 1.先把参数保存起来
	int tmp_num;
	if (num_param <= 6) {
		tmp_num = num_param;
	}else{
		tmp_num = 6;
		int len = num_param - 6;
		long *param_stack = (long *)calloc(len,8);
		int i,j = 0;
		for(i = (num_param - 1); i >= 6; i--,j++) {
			*(param_stack + j) = paramers[i];
		}
		for(int k = 0; k < len; k++) {
			printf("*(param_stack + %d) = %ld \t",k,*(param_stack + k));
		}
		puts("");
		// 第6个以后的参数保存到栈中
		push_stack(pid, &(regs.rsp), param_stack, len);
	}

	// 前6个参数保存在寄存器里
	switch(tmp_num) {
		case 6:
			regs.r9 = paramers[5];
		case 5:
			regs.r8 = paramers[4];
		case 4:
			regs.rcx = paramers[3];
		case 3:
			regs.rdx = paramers[2];
		case 2:
			regs.rsi = paramers[1];
		case 1:
			regs.rdi = paramers[0];
			break;
		case 0:
			{
				printf("no paramer ...\n" );
			}
	}
	//2.把当前指令的下一条指令入栈,函数往上一级返回的时候要用到
	// rip = rip -1 是为了让函数重新撞击断点，断点指令0xcc 就1 Byte,当上次撞击断点时 pc 已经指下了断点的下一条指令
	long rip[1] = {regs.rip - 1};
	push_stack(pid, &(regs.rsp),rip, 1);

	//3. 把pc指向目标函数首地址
	regs.rip = func_addr;
	set_registers(pid,&regs);
	ptrace_cont(pid);
	
	/*
	 * 为了获取 调用的目标函数的返回值(其实这个返回值存放在rax中)，只能让函数继续运行，直到运行结束 再次撞击原来的断点。
	 这时才是正解的时机获取到 目标函数的返回值 rax
	 * **/
	wait_breakpoint(pid);
	if(result != NULL) {
		struct user_regs_struct call_ret_regs;
		get_registers(pid, &call_ret_regs);
		*result = call_ret_regs.rax;
		//printf("call_function return value = %ld\n",*result);
	}

	return 0;
}

// 调用 mmap 建立匿名映射空间
long long call_mmap(pid_t pid,unsigned long size) {
	size_t func_addr = get_vaddr(pid, "mmap", "libc-");
	printf("mmap:          		0x%lx\n", func_addr);

	uint8_t num_param = 6;
	long parameters[num_param];
	parameters[0] = 0;				// 设置为NULL表示让系统自动选择分配内存的地址
	parameters[1] = size;				// 映射内存的大小
	parameters[2] = PROT_READ | PROT_WRITE | PROT_EXEC; // 表示映射内存区域可读可写可执行
	parameters[3] = MAP_PRIVATE | MAP_ANONYMOUS;	// 建立匿名映射
	parameters[4] = 0;				// 若需要映射文件到内存中，则为文件的fd
	parameters[5] = 0;				// 文件映射偏移量

	long long return_value;
	call_function(pid, func_addr, parameters, num_param,&return_value);
	printf("%s %d return_value = 0x%llx\n",__FUNCTION__,__LINE__,return_value);

	return return_value;
}

// 调用 munmap 解除一个映射关系
int call_munmap(pid_t pid,long *parameters, uint8_t num_param) {
	size_t func_addr = get_vaddr(pid, "munmap", "libc-");
	printf("munmap:                   0x%lx\n", func_addr);

	call_function(pid, func_addr, parameters, num_param,NULL);

	return 0;
}

// 往进程中注入动态库
unsigned long inject_library(pid_t pid, char * lib_path) {
        // 先获取动态库
	int fd;
        void * start;
        struct stat sb;
        fd = open(lib_path, O_RDONLY); /* 打开/etc/passwd */
        fstat(fd, &sb); /* 取得文件大小 */
        start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if(start == MAP_FAILED) /* 判断是否映射成功 */
        {
                perror("mmap init fail");
                exit(-1);
        }

	// 在子进程中分配一片空间，用来写入需要注入的动态库
	unsigned long module_addr = call_mmap(pid,sb.st_size);

	// 把动态库写入分配的空间中
	putdata(pid, module_addr, start, sb.st_size);

	munmap(start, sb.st_size); /* 解除映射 */
	close(fd);

	return module_addr;
}

