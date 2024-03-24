#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dlfcn.h>

#include "ptrace_util.h"
#include "vaddr_by_symbol.h"


//long long get_return(pid_t pid) {
//	wait_breakpoint(pid);
//        struct user_regs_struct regs;
//        get_registers(pid,&regs);
//	return regs.rax;
//}

long long call_munmap(pid_t pid,long *parameters, const unsigned int num_param) {
	size_t func_addr = get_vaddr(pid, "munmap", "libc-");
	printf("munmap:                   0x%lx\n", func_addr);

	call_function(pid, func_addr, parameters, num_param,NULL);
	
	return 0;
}

long long call_mmap(pid_t pid) {
	size_t func_addr = get_vaddr(pid, "mmap", "libc-");
	printf("mmap:          		0x%lx\n", func_addr);

	int fd;
	void * start;
	struct stat sb;
	fd = open("./libinject.so", O_RDONLY); /* 打开/etc/passwd */
	fstat(fd, &sb); /* 取得文件大小 */
	start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(start == MAP_FAILED) /* 判断是否映射成功 */
	{
		perror("mmap init fail");
		exit(-1);
	}
//	printf("start = %p, value = %s\n",start,(char *)start);
//	munmap(start, sb.st_size);
//	close(fd);

	const unsigned int num_param = 6;
	long parameters[num_param];
	parameters[0] = 0;				// 设置为NULL表示让系统自动选择分配内存的地址
	parameters[1] = sb.st_size;		// 映射内存的大小
	parameters[2] = PROT_READ | PROT_WRITE | PROT_EXEC; // 表示映射内存区域可读可写可执行
	parameters[3] = MAP_PRIVATE | MAP_ANONYMOUS;	// 建立匿名映射
	//int fd2 = (int)call_open(pid);
	parameters[4] = 0;				// 若需要映射文件到内存中，则为文件的fd
	parameters[5] = 0;				// 文件映射偏移量

	long long return_value;
	call_function(pid, func_addr, parameters, num_param,&return_value);
	printf("%s %d return_value = 0x%llx\n",__FUNCTION__,__LINE__,return_value);


	unsigned long mmap_make_vaddr = (unsigned long)return_value;
	// uint8_t *test_data = "./libcaculate.so";
	// unsigned long len2 = strlen(test_data) + 1;
	// putdata(pid, mmap_make_vaddr, test_data, len2);
	// printf("%d\n",__LINE__);
	putdata(pid, mmap_make_vaddr, start, sb.st_size);

//	unsigned long len = sb.st_size;	
//	uint8_t data[len];
//	getdata(pid, mmap_make_vaddr, data, len);
//	printf("data -> %p:\n%s\n",(void *)mmap_make_vaddr,data);
//	for(int i = 0;i < len; i++) {
//		printf("%02x ", data[i]);
//	}
//	puts("");

	return return_value;
}


void call_dlerror(pid_t pid) {
	size_t func_addr = get_vaddr(pid, "dlerror", "libc-");
	printf("dlerror:                 0x%lx\n", func_addr);

	const unsigned int num_param = 0;
	long parameters[num_param];

	long long return_value;
	call_function(pid, func_addr, parameters, num_param,&return_value);
	printf("%s %d return_value = 0x%llx\n",__FUNCTION__,__LINE__,return_value);
}

long long call_dlopen(pid_t pid) {
	size_t func_addr = get_vaddr(pid, "dlopen", "libc-");
	printf("dlopen:          	0x%lx\n", func_addr);
	const unsigned int num_param = 2;
	long parameters[num_param];
	//parameters[0] = 0x4020e8;
	long long str_addr = call_mmap(pid);
	parameters[0] = str_addr;
	parameters[1] = RTLD_NOW | RTLD_LAZY;

	long long return_value;
	call_function(pid, func_addr, parameters, num_param,&return_value);
	printf("%s %d return_value = 0x%llx\n",__FUNCTION__,__LINE__,return_value);

	if (return_value == 0x0) {
		call_dlerror(pid);
	}

	return return_value;
}

long long call_open(pid_t pid) {
	size_t func_addr = get_vaddr(pid, "open", "libc-");
	printf("open:          		0x%lx\n", func_addr);
	const unsigned int num_param = 2;
	long parameters[num_param];
	parameters[0] = 0x4020e2;;
	parameters[1] = O_RDONLY | O_WRONLY;

	long long return_value;
	call_function(pid, func_addr, parameters, num_param,&return_value);
	printf("%s %d return_value = 0x%llx\n",__FUNCTION__,__LINE__,return_value);

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
	const unsigned int num_param = 2;
	long parameters[num_param];
	parameters[0] = 0x4;
	parameters[1] = 0x4;

	long long return_value;
	call_function(pid,func2Addr,parameters,num_param,&return_value);
	printf("%s %d return_value = 0x%llx\n",__FUNCTION__,__LINE__,return_value);
}

void call_func0_so(pid_t pid,unsigned long module_addr){
	size_t func_addr = module_addr + 0x1108;
	const unsigned int num_param = 1;
	long parameters[num_param];
	parameters[0] = 100;

	long long return_value;
	call_function(pid,func_addr,parameters,num_param,&return_value);
	printf("%s %d return_value = 0x%llx\n",__FUNCTION__,__LINE__,return_value);
}

void call_print(pid_t pid) {
	size_t funcAddr = 0x401357;
	const unsigned int num_param = 1;
	long parameters[num_param];
	parameters[0] = 0x4020e8;
	call_function(pid,funcAddr,parameters,num_param,NULL);
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
	unsigned long module_addr = call_mmap(pid);
	call_func0_so(pid,module_addr);
//	call_dlopen(pid);
//	wait_breakpoint(pid);
//	call_print(pid);

	recovery_breakpoint(pid,backup_regs);

	return 0;
}
