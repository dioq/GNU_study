# 查找

## find 在目录下查找文件

### 根据文件名字查找

find [directory] -name [target file name]
如: find ./ -name "*.m"   # 查找后缀是 .m 的文件

### 根据文件大小查找

find [file path] -size [sizes]
如: find ./ -size +100M   # 查找大于 100M 的文件

## 查找到后执行其他命令

查找到后直接复制到 ./test 目录下
find ./ -name "*.c" -exec cp '{}' ./test/ \;
查找到后直接删除
find ./ -name "*.c" -exec rm -rf '{}' \;

## grep 根据文件内容查找

grep [option]  "context" path
option:
  -r                   递归
  -i                   忽略大小写
  -n                   输出行号（显示原始文件中的行号）
  -v                   反向查找
  --color=auto         搜索出的关键字用颜色显示
