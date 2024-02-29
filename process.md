# 进程

## ps (process status)查看进程状态

options:
-a 显示一个终端的所有进程,除了会话引线
-u uid or username 选择有效的用户id或者是用户名
-x 显示没有控制终端的进程,同时显示各个命令的具体路径

常用组合: ps aux

## pidof 根据进程名获取进程 id

pidof  [process_name]
