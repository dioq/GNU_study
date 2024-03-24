#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <dlfcn.h>

int global_var = 0x1;

int func0()
{
	printf("Hello, this is func0 ...\n");
	return 0xFF00;
}

int func1(int p1, int p2)
{
	printf("Hello, this is func1 ...\n");
	return global_var + p1 + p2;
}

int func2(int p1, int p2)
{
	printf("Hello, this is func2 ...\n");
	return p1 * p2;
}

void read_mem(unsigned long vaddr, unsigned long len)
{
	printf("Hello, this is read_mem ...\n");
	uint8_t *tmp_addr = (uint8_t *)vaddr;
	for (unsigned long i = 0; i < len; i++)
	{
		printf("%02x ", *(tmp_addr + i));
	}
	puts("");
}

int func10(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10)
{
	printf("Hello, this is func10 ...\n");
	printf("p1 = %d,p2 = %d,p3 = %d,p4 = %d,p5 = %d,p6 = %d,p7 = %d,p8 = %d,p9 = %d,p10 = %d\n",
		   p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
	return p1 + p10;
}

char *path = "/home/dio/Repositories/C_study/ptrace_study/tracee/libcaculate.so";
void print(char *str)
{
	puts(str);
}

void sighandler(int);

/*
 * gcc -no-pie tracee.c -o tracee
 * **/
int main()
{
	//	signal(SIGINT, sighandler);

	const int len = 64;
	uint8_t *mem_space = (uint8_t *)malloc(len);
	memset(mem_space, 'a', 32);
	//	int fd = open("./loaded_file", O_RDONLY | O_WRONLY);
	// 打开动态链接库
	//	void *handle = dlopen(path, RTLD_LAZY);
	//	printf("handle = %p\n",handle);
	// char c;
	while (1)
	{
		printf("****************** pid:%d *******************\n", getpid());
		// printf("input commond:");
		// c = getchar();
		printf("mem_space addr:%p,value:\n", mem_space);
		for (int i = 0; i < len; i++)
		{
			printf("%02x ", *(mem_space + i));
		}
		puts("");
		//		printf("fd = %d\n", fd);
		// read_mem((unsigned long)mem_space,len);
		//		printf("open            addr: %p\n", &open);
		printf("mmap            addr: %p\n", &mmap);
		printf("dlopen          addr: %p\n", &dlopen);
		printf("printf          addr: %p\n", &printf);
		// printf("&func0          addr: %p\n", &func0);
		printf("&func1          addr: %p\n", &func1);
		printf("&func2          addr: %p\n", &func2);
		printf("&read_mem       addr: %p\n", &read_mem);
		printf(" path           addr: %p\n", path);
		printf("&print          addr: %p\n", &print);
		// printf("&func10         addr: %p\n", &func10);
		global_var++;
		int ret = 0;
		ret = func1(1, 1);
		printf("result = 0x%x\n", ret);
		sleep(5);
	}

	return 0;
}

void sighandler(int signum)
{
	printf("signal %d, exit ...\n", signum);
	exit(1);
}
