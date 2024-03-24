#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "ptrace_util.h"
#include "vaddr_by_symbol.h"

int call_mmap(pid_t pid, struct user_regs_struct breakpoint_regs, long *result)
{
	printf("%s %d\n", __FILE__, __LINE__);
	size_t func_addr = get_vaddr(pid, "mmap", "libc-");
	//printf("func_addr:          0x%lx\n", func_addr);
	const unsigned int num_param = 6;
	//	void * start;
	//	struct stat sb;
	//	int fd = open("./loaded_file", O_RDONLY | O_WRONLY);
	//	fstat(fd, &sb);
	//start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	long parameters[num_param];
	parameters[0] = 0;									// 设置为NULL表示让系统自动选择分配内存的地址
	parameters[1] = 0x1000;								// 映射内存的大小
	parameters[2] = PROT_READ | PROT_WRITE | PROT_EXEC; // 表示映射内存区域可读可写可执行
	parameters[3] = MAP_ANONYMOUS | MAP_PRIVATE;		// 建立匿名映射
	parameters[4] = 0;									//  若需要映射文件到内存中，则为文件的fd
	parameters[5] = 0;									//文件映射偏移量

	call_function(pid, func_addr, parameters, num_param, breakpoint_regs, result);
	printf("mmap return vaddr = 0x%lx\n", *result);

	return 0;
}

// int write_char_table(pid_t pid, unsigned long vaddr) {
// 	uint8_t char_table[] = {'H','e','l','l','o','\0','W','o','r', 'l','l','d','\0'};
// 	unsigned long len = sizeof(char_table);

// 	putdata(pid, vaddr, char_table, len);

// 	return 0;
// }

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage:\n\t%s pid\n", argv[0]);
		return -1;
	}
	pid_t pid = atoi(argv[1]);

	ptrace_attach(pid);
	int status;
	wait(&status);

	//	struct user_regs_struct current_regs;
	//	get_registers(target_pid, &current_regs);
	//	show_registers(current_regs);
	size_t func1Addr = 0x401211;
	set_breakpoint(pid, func1Addr);

	ptrace_cont(pid);

	struct user_regs_struct breakpoint_regs;
	if (is_hit_breakpoint(pid, &breakpoint_regs) == -1)
	{
		printf("%s:%d not hit breakpoint...\n", __FILE__, __LINE__);
		return -1;
	}
	show_registers(breakpoint_regs);
	//getchar();

	//	size_t func_addr = get_vaddr(target_pid,"dlopen", "libc-");
	//	printf("func_addr:          0x%lx\n", func_addr);
	//	const unsigned int num_param2 = 2;
	//	long parameters2[num_param2];
	//	parameters2[0] = 0x8ee2a0;//result;;
	//	parameters2[1] = 0x10;//sb.st_size;
	//	call_function(target_pid,read_mem,parameters2,num_param2,breakpoint_regs,NULL);

	long mmap_vaddr;
	call_mmap(pid, breakpoint_regs, &mmap_vaddr);
	printf("%s %d\n", __FILE__, __LINE__);

	uint8_t char_table[] = {'H', 'e', 'l', 'l', 'o', '\0', 'W', 'o', 'r', 'l', 'l', 'd', '\0'};
	unsigned long len = sizeof(char_table);
	putdata(pid, (unsigned long)mmap_vaddr, char_table, len);
	printf("%s %d\n", __FILE__, __LINE__);

	// uint8_t dst[len];
	// getdata(pid, (unsigned long)mmap_vaddr, dst, len);
	// printf("file size : %lu\n", len);
	// for (int k = 0; k < len; k++)
	// {
	// 	printf("%02x ", *(dst + k));
	// }
	// puts("");

	// ptrace_cont(pid);
	set_breakpoint(pid, func1Addr);
	ptrace_cont(pid);
	if (is_hit_breakpoint(pid, NULL) == -1)
	{
		printf("%s:%d not hit breakpoint...\n", __FILE__, __LINE__);
		return -1;
	}

	size_t read_mem = 0x401273;
	const unsigned int num_param2 = 2;
	long parameters2[num_param2];
	parameters2[0] = mmap_vaddr; //result;;
	parameters2[1] = len;		 //sb.st_size;
	call_function(pid, read_mem, parameters2, num_param2, breakpoint_regs, NULL);
	printf("%s %d\n", __FILE__, __LINE__);

	//	size_t func2Addr = 0x401261;
	//	const unsigned int num_param2 = 2;
	//	long parameters2[num_param2];
	//	parameters2[0] = 0x100;
	//	parameters2[1] = 0x100;//sb.st_size;
	//	call_function(target_pid,func2Addr,parameters2,num_param2,breakpoint_regs,NULL);

	// uint8_t dst[len];
	// getdata(pid, (unsigned long)mmap_vaddr, dst, len);
	// printf("file size : %lu\n", len);
	// for (int k = 0; k < len; k++)
	// {
	// 	printf("%02x ", *(dst + k));
	// }
	// puts("");

	// ptrace_cont(pid);

	return 0;
}
