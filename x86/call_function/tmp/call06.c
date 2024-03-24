#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dlfcn.h>

#include "ptrace_util.h"
//#include "vaddr_by_symbol.h"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage:\n\t%s pid\n", argv[0]);
		return -1;
	}
	pid_t pid = atoi(argv[1]);

	ptrace_attach(pid);

	struct user_regs_struct backup_regs, regs;
	get_registers(pid,&backup_regs);

//	long rip[1] = {backup_regs.rip};
//	push_stack(pid, &(backup_regs.rsp),rip, sizeof(rip));


	memcpy(&regs,&backup_regs, sizeof(struct user_regs_struct));
	unsigned long func2_addr = 0x401241;
//	regs.rip = func2_addr;
      
//	regs.rdi = 100;
//	regs.rsi = 100;

	set_registers(pid,&regs);

	ptrace_cont(pid);
	
	return 0;
}
