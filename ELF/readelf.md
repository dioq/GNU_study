# 查看ELF文件

readelf -s <ELF file>              # 查看符号表中的信息 Symbol table '.symtab' '.dynsym'
readelf --dyn-syms  <ELF file>     # 查看动态符号表中的信息 symbol table '.dynsym'
readelf -r <ELF file>              # 查看动态链接重定位表 relocation section '.rela.dyn' '.rela.plt'
readelf -d <ELF file>              # 查询 dynamic section 
readelf -S <ELF file>              # 查看 section headers
readelf -a <ELF file>              # 查看所有信息

ldd <ELF file>                     # 查看 <ELF file> 所有依赖的共享库链及共享库路径
