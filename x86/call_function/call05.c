#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dlfcn.h>

#include "ptrace_util.h"
#include "vaddr_by_symbol.h"

void init_symbol(pid_t pid,unsigned long module_addr) {
	char * inject_library_path = "./libinject.so";
	void *handle = load_module(inject_library_path);
	char *funcName = "init_symbol";
	char *moduleName = "libinject.so";
	unsigned long inject_func_offset = get_func_offset(funcName,moduleName,handle);
	printf("inject_func_offset:0x%lx\n", inject_func_offset);

	unsigned long func_addr = module_addr + inject_func_offset;//0x10f9;

	uint8_t num_param = 1;
	long parameters[num_param];
	unsigned long printf_vaddr = get_vaddr(pid,"printf","libc-");
	printf("printf_vaddr:0x%lx\n", printf_vaddr);
	parameters[0] = printf_vaddr;
	//	parameters[1] = 0x100;
	//	parameters[2] = 0x200;
	long long return_value;
	call_function(pid,func_addr,parameters,num_param,&return_value);
	printf("%s %d return_value = 0x%llx\n",__FUNCTION__,__LINE__,return_value);
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage:\n\t%s pid\n", argv[0]);
		return -1;
	}
	pid_t pid = atoi(argv[1]);

	ptrace_attach(pid);

	//char * target_process_image_path = "./tracee";
	//void *handle = load_module(target_process_image_path);
	//char *funcName = "func1";
	//char *moduleName = "tracee";
	//unsigned long target_func_addr = get_vaddr(pid,funcName,moduleName,handle);
	//printf("target_func_addr:0x%lx\n",target_func_addr);
	
	size_t func1Addr = 0x401201;
	set_breakpoint(pid, func1Addr);
	ptrace_cont(pid);

	wait_breakpoint(pid);
	struct user_regs_struct backup_regs;
	get_registers(pid,&backup_regs);

	char * lib_path = "./libinject.so";
	unsigned long module_addr = inject_library(pid,lib_path);
	printf("module_addr:0x%lx\n",module_addr);


	init_symbol(pid,module_addr);

//	unsigned long vaddr = module_addr + 0x4028;
//	union {
//		long val;
//		uint8_t bytes[8];
//	}data;
//	data.val = 0x7f8a412eee10;
//	putdata(pid, vaddr, data.bytes, 6);

//	size_t func_addr = module_addr + 0x116a;
//	uint8_t num_param = 2;
//	long parameters[num_param];
//	parameters[0] = 0x200;
//	parameters[1] = 0x100;
////	parameters[2] = 0x200;
//	long long return_value;
//	call_function(pid,func_addr,parameters,num_param,&return_value);
//	printf("%s %d return_value = 0x%llx\n",__FUNCTION__,__LINE__,return_value);


	recovery_breakpoint(pid,backup_regs);

	return 0;
}
