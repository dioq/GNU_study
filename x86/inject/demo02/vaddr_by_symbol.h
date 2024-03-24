#include <stdio.h>
#include <unistd.h>



// 计算模块在进程中的虚拟地址(即so加载到进程后的首地址)
size_t get_module_base(pid_t pid, char *moduleName);
// 计算 函数 在模块中的偏移(模块必需在当前进程中，也即动态库xxx.so 被当前进程加载进来了。否则在当前进程中则找不到这个模块)
size_t get_func_offset(char *funcName,char *moduleName);
// 计算模块中 symbol 在进程中的虚拟地址
size_t get_vaddr(pid_t pid,char *funcName,char *moduleName);
