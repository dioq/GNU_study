#include "vaddr_by_symbol.h"

// 计算模块在进程中的虚拟地址(即so加载到进程后的首地址)
unsigned long get_module_base(int pid, char *moduleName)
{
	unsigned long moduleAddr;
	char * buf;
	char * end;
	char mapfile[0x18];
	sprintf(mapfile, "/proc/%d/maps", pid);
	FILE * fd = fopen(mapfile, "r");
	if(!fd)
	{
		printf("open maps error!");
		exit(1);
	}
	//search the libc-.....
	buf = (char*) malloc(0x100);
	do{
		fgets(buf, 0x100, fd);
	} while(!strstr(buf, moduleName));
	end = strchr(buf, '-');
	moduleAddr = strtol(buf, &end, 16);
	printf("The process %d's module(%s) base is: 0x%lx\n", pid, moduleName, moduleAddr);
	fclose(fd);

	return moduleAddr;
}


// 计算 函数 在模块中的偏移(模块必需在当前进程中，也即动态库xxx.so 被当前进程加载进来了。否则在当前进程中则找不到这个模块)
unsigned long get_func_offset(char *funcName,char *moduleName,void *handle)
{
	//get function offset from self process, the shared libc.so
	unsigned long funcAddr = (size_t)dlsym(handle, funcName); // 获取当前进程中 名为funcName 的函数 所在的 虚拟地址 vaddr
	unsigned long moduleAddr = get_module_base(getpid(),moduleName);
	unsigned long offset     = funcAddr - moduleAddr; // 当前函数地址 减去 函数所在模块(即加载的libc-2.31.so)在进程中的虚拟地址 即得到 函数在模块内的偏移
	printf("function:%s in module:%s\n",funcName,moduleName);
	printf("funcAddr: 		0x%lx\n",funcAddr);
	printf("moduleAddr: 		0x%lx\n",moduleAddr);
	printf("offset: 		0x%lx\n",offset);

	return offset;
}

// 把动态库加载到进程
void * load_module(char *lib_path) {
	void *handle;
	//打开动态链接库
	handle = dlopen(lib_path, RTLD_LAZY);
	//printf("handle = %p\n",handle);
	if (!handle) {
		fprintf(stderr, "%s\n", dlerror());
		exit(-1);
	}

	//清除之前存在的错误
	dlerror();

	//关闭动态链接库,关闭后该动态库就会从 当前进程中 移除
//	dlclose(handle);

	return handle;
}

// 计算系统模块(libc-)中 symbol 在进程中的虚拟地址
unsigned long get_vaddr(pid_t pid,char *funcName,char *moduleName) {
        size_t base = get_module_base(pid,moduleName);
        //printf("base:           0x%lx\n", base);

        size_t offset = get_func_offset(funcName,moduleName,NULL);
        //printf("offset:         0x%lx\n", offset);

        size_t vaddr = base + offset; //  模块在目标进程中的基址 加上函数在模块内的偏移 就是函数在目标进程中的虚拟地址 
        //printf("vaddr:          0x%lx\n", vaddr);
	
	return vaddr;
}

