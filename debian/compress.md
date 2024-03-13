# 压缩和解压命令

## tar

参数:
-c                           # 压缩指令
-x                           # 解压缩指令
-C [DIR]                     # directory 解压缩输出目录
-v                           # 显示操作过程
-p                           # 包括文件权限,并应用到所有文件
-f                           # file 指定待压缩文件
--exclude                    # 排除指定目录,不进行压缩
-z                           # 使用 gzip 压缩格式
-j                           # 使用 bzip2 压缩格式

压缩例子:
tar -czvpf test.tar.gz ./dir/ ./dir2/test.txt
解压缩例子:
tar -xzvpf test.tar.gz -C ./

## pigz 多线程优化 tar 速度

使用 pigz多线程提高压缩和解压速度(sudo apt install pigz)

### 压缩

tar --use-compress-program=pigz -czvpf test.tar.gz ./dir/ ./dir2/test.txt

### 解压

tar --use-compress-program=pigz -xzvpf test.tar.gz -C ./

## zip

参数:
-r                        # 递归
-q                        # 不显示操作过程
-d                        # 解压缩输出位置
压缩例子:
zip -rq new.zip ./dir/ ./dir2/test.txt       # 将 ./dir/ ./dir2/test.txt 压缩成 new.zip包
解压缩例子:
unzip -q new.zip -d ./                       # 解压 new.zip 到 ./ 目录
