# GCC 编译器

options:
-E                      预处理(preprocessing)
-S                      编译(compilation)
-c                      汇编(assembly),生成 object file 目标文件
-o                      输出文件
-g                      在目标文件中添加调试信息,便于gdb调试或objdump反汇编
-s                      删除目标文件中的符号表
-L                      编译时指定共享库查找路径,如指定当前目录 gcc -L./
-l                      编译时指定要链接的共享库的名称,如库的名称是libtest.a,则 -ltest 即去头去尾
-shared                 编译成动态库
-fPIC                   生成位置独立代码,一般 -shared -fPIC 一起使用编译生成动态库
-static                 链接静态库,不添加默认链接动态库
-Wl                     将指定的参数传递给链接器,如:-Wl,-rpath=<path> 修改 DT_RPATH 的值,指定运行时共享库查找路径
-no-pie                 不使用 PIE (position-independent executable)
-rdynamic               通知链接器将所有符号添加到动态符号表中
-nostdinci              使编译器不在系统缺省的头文件目录里面找头文件,一般和-L联合使用,明确限定头文件的位置

## 使用

引用的头文件不需要输入给gcc,编译器会递归展开头文件
gcc命令行格式: gcc x1.c x2.c -o main

## PIE

PIE (position-independent executable) 是一种生成地址无关可执行程序的技术。如果编译器在生成可执行程序的过程中使用了PIE,那么当可执行程序被加载到内存中时其加载地址存在不可预知性。
