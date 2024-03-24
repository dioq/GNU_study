#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "ptrace_util.h"
#include "vaddr_by_symbol.h"


long long get_return(pid_t pid) {
	wait_breakpoint(pid);
        struct user_regs_struct regs;
        get_registers(pid,&regs);
	return regs.rax;
}

long long call_mmap(pid_t pid) {
	size_t func_addr = get_vaddr(pid, "mmap", "libc-");
	printf("func_addr:          0x%lx\n", func_addr);
	const unsigned int num_param = 6;
	void * start;
	struct stat sb;
	int fd = open("./loaded_file", O_RDONLY | O_WRONLY);
	fstat(fd, &sb);
	//start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	long parameters[num_param];
	parameters[0] = 0;				// 设置为NULL表示让系统自动选择分配内存的地址
	parameters[1] = sb.st_size;//0x1000;		// 映射内存的大小
	parameters[2] = PROT_READ | PROT_WRITE | PROT_EXEC; // 表示映射内存区域可读可写可执行
	parameters[3] = MAP_PRIVATE | MAP_ANONYMOUS;	// 建立匿名映射
	parameters[4] = 3;				// 若需要映射文件到内存中，则为文件的fd
	parameters[5] = 0;				// 文件映射偏移量

	long long return_value;
	call_function(pid, func_addr, parameters, num_param,&return_value);
	// long long return_value = get_return(pid);
	printf("return_value = 0x%llx\n", return_value);

	//uint8_t *test_data = "hello hope you be success";
	//unsigned long len = strlen(test_data);
	//putdata(pid, func_addr, test_data, len);
	unsigned long len = sb.st_size;
	uint8_t data[len];
	getdata(pid, func_addr, data, len);
	printf("data:%s\n",data);
	for(int i = 0;i < len; i++) {
		printf("%02x ", data[i]);
	}
	puts("");
	
	return return_value;
}

// int write_char_table(pid_t pid, unsigned long vaddr) {
// 	uint8_t char_table[] = {'H','e','l','l','o','\0','W','o','r', 'l','l','d','\0'};
// 	unsigned long len = sizeof(char_table);

// 	putdata(pid, vaddr, char_table, len);

// 	return 0;
// }

void call_func2(pid_t pid) {
	size_t func2Addr = 0x401241;
	const unsigned int num_param2 = 2;
	long parameters2[num_param2];
	parameters2[0] = 0x4;
	parameters2[1] = 0x4;//sb.st_size;
	long long return_value;
	call_function(pid,func2Addr,parameters2,num_param2,&return_value);
	// long long return_value = get_return(pid);
	// call_function(pid, func_addr, parameters, num_param,&return_value);
	printf("return_value = 0x%llx\n", return_value);
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

	size_t func1Addr = 0x401211;
	set_breakpoint(pid, func1Addr);
	ptrace_cont(pid);

	wait_breakpoint(pid);
	struct user_regs_struct backup_regs;
	get_registers(pid,&backup_regs);


	//call_func2(pid);
	call_mmap(pid);

	recovery_breakpoint(pid,backup_regs);
	
	return 0;
}
