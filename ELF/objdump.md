# objdump 反汇编

objdump -d <ELF file>    #显示文件
objdump -f <ELF file>    #显示文件头信息
objdump -D <ELF file>    #反汇编所有section
objdump -h <ELF file>    #显示目标文件各个section的头部摘要信息
objdump -x <ELF file>    #显示所有可用的头信息，包括符号表、重定位入口
objdump -i <ELF file>    #显示可用架构和目标格式列表
objdump -r <ELF file>    #显示文件的重定位入口
objdump -R <ELF file>    #显示文件的动态重定位入口，仅仅对于动态目标文件有意义
objdump -S <ELF file>    #尽可能反汇编出源代码,与-d类似
objdump -t <ELF file>    #显示文件的符号表入口
