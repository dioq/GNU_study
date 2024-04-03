# port端口

查看已经连接的服务端口(ESTABLISHED)
netstat -a
查看所有的服务端口(LISTEN,ESTABLISHED)
netstat -ap
检查正在监听的端口
netstat -lnp | grep 8080
也可以使用lsof命令:
lsof -i:8080
