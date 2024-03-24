#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
typedef int(*Strcmp)(const char*, const char*);
int strcmp(const char* s1, const char* s2)
{
	static void* handle = NULL;
	static Strcmp org_strcmp = NULL;
	if(!handle)
	{ //解析得到真实的strcmp函数
		handle = dlopen("libc.so.6", RTLD_LAZY);
		org_strcmp = (Strcmp)dlsym(handle, "strcmp");
	}
	//做我们想做的
	printf("Hacked by way of ld_preload\n\n\n");
	//完成真实的功能
	return org_strcmp(s1, s2);
}
