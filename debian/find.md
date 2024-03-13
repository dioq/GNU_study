# 查找

## find 在 xxx 目录下查找文件

### 根据文件名字查找

find [directory] -name [target file name]
如: find ./ -name "*.m"   # 查找后缀是 .m 的文件

### 根据文件大小查找

find [file path] -size [sizes]
如: find ./ -size +100M   # 查找大于 100M 的文件

## 查找到后执行其他命令

查找到后直接复制到 ./test 目录下
find ./basic_code/ -name "*.c" -exec cp '{}' ./test/ \;
查找到后直接删除
find ./test/ -name "*.c" -exec rm '{}' \;
