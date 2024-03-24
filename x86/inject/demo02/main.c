#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dlfcn.h>

#include "ptrace_util.h"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage:\n\t%s pid\n", argv[0]);
		return -1;
	}
	pid_t pid = atoi(argv[1]);

	ptrace_attach(pid);

	size_t func1Addr = 0x401211;
	set_breakpoint(pid, func1Addr);
	ptrace_cont(pid);

	wait_breakpoint(pid);
	struct user_regs_struct backup_regs;
	get_registers(pid,&backup_regs);

	// 注入动态库
	char * lib_path = "./libinject.so";
	unsigned long module_addr = inject_library(pid,lib_path);
	printf("module_addr:0x%lx\n",module_addr);

	// 调用动态库里的方法
	size_t func_addr = module_addr + 0x1108;
	uint8_t num_param = 1;
	long parameters[num_param];
	parameters[0] = 1000;
	long long return_value;
	call_function(pid,func_addr,parameters,num_param,&return_value);
	printf("%s %d return_value = 0x%llx\n",__FUNCTION__,__LINE__,return_value);


	recovery_breakpoint(pid,backup_regs);

	return 0;
}
