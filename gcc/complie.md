# GCC 编译过程

编译过程一般分为四个阶段:
预处理(preprocessing)
编译(compilation)
汇编(assembly)
连接(linking)

## 1. 预编译

1.#define 宏文本替换
2.#include 递归展开头文件
3.#if #endif #elif 删除预编译指令
4.删除注释
5.添加行号和文本标识
6.保留 #pragma 编译器处理

gcc -E xxx.c -o xxx.i

## 2. 编译

1.词法分析
2.语法分析
3.语义分析
4.代码优化
生成汇编译代码

gcc -S xxx.i -o xxx.s

## 3. 汇编

将汇编代码转化成机器码,生成的二进制文件就是目标文件(object file)

gcc -c xxx.s -o xxx.o

注:每一个源文件(.c)文件只能生成一个目标文件(.o)文件,不存在多个 .c 生成 一个 .o 文件的情况

## 4. 链接

目标文件(object file)链接成可执行文件
1.合并相似节
如: .text section 合并到输出文件的 .text section
2.符号解析
3.符号的重定位
text段 虚假地址改真实,纠正虚假偏移
4.分配地址和空间

ld -e main -o run main.o sum.o
