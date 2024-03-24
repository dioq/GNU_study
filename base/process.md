# 进程

## ps (process status)查看进程状态

options:
-a      显示一个终端的所有进程,除了会话引线
-u      uid or username 选择有效的用户id或者是用户名
-x      显示没有控制终端的进程,同时显示各个命令的具体路径
-e      显示所有进程
-f      显示程序间的关系

常用组合:
ps aux
ps -ef

## 根据进程名获取进程 id

通过 grep 获取 pid 的方法为(-v grep 是为了避免匹配到 grep 进程)
ps -ef | grep "name" | grep -v grep | awk '{print $2}'

最简单的方法是使用 pgrep
pgrep -f name
