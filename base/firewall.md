# 防火墙管理

sudo apt-get install ufw

## 防火墙状态，如果输出显示 Status: inactive，表示 UFW 处于禁用状态

sudo ufw status

## 关闭防火墙

sudo ufw disable

## 启动防火墙

sudo ufw enable

## 设置防火墙默认策略的命令，将所有出站连接允许通过防火墙

sudo ufw default allow outgoing

## 设置防火墙默认策略的命令，将所有入站连接禁止通过防火墙

sudo ufw default deny incoming

## 允许端口

sudo ufw allow 8000

## 关闭特定端口

sudo ufw delete allow 8000

## 指定特定来源的源 IP 地址

sudo ufw allow from <IP 地址> to any port <端口号>

## 如配置 172.16.2.193 设备能访问或关闭服务器的 8000 端口

sudo ufw allow from 172.16.2.193 to any port 8000
sudo ufw delete allow from 172.16.2.193 to any port 8000

## 允许来自 192.168.1.100 IP 地址的 HTTP 流量通过防火墙

sudo ufw allow from 172.16.2.193 to any port 3336 proto tcp
sudo ufw delete allow from 172.16.2.193 to any port 3336 proto tcp

## 重新加载防火墙规则

sudo ufw reload

## 重置防火墙配置规则

sudo ufw reset
