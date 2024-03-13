# 系统备份和迁移

## 1.备份原系统(在原系统上操作)

sudo tar -czvpf backup.tar.gz  --exclude=/proc --exclude=/sys --exclude=/run --exclude=/mnt --exclude=/tmp --exclude=/media --exclude=/lost+found /

## 2.迁移系统(在目标系统上操作)

### 2.1 在根目录/下创建文件夹,并将原有系统所在的磁盘分区挂载到该文件夹上

mkdir restore      # 创建临时挂载文件
df -h        # 查看分区,找出根目录/ 所挂载的磁盘分区
mount /dev/sdaX /restore   # 假定原有系统在磁盘分区/dev/sdaX上

### 2.2 保存目标系统与分区相关的文件

cp /restore/boot/grub/grub.cfg /tmp
cp /restore/etc/fstab /tmp
若有/etc/fstab.d 就一并复制
注: 我这里是保存到 /tmp,最好是直接保存到移动磁盘里.移动硬盘挂载的文件地址(/media/....)

### 2.3 把备份系统的压缩文件解压

sudo tar -xzvpf backup.tar.gz -C /restore

### 2.4 还原目标系统分区相关文件

cp grub.cfg /restore/boot/grub/
cp fstab /restore/etc/

### 2.5 引导修复

sudo add-apt-repository ppa:yannubuntu/boot-repair         # 添加源
sudo apt-get update
sudo apt-get install -y boot-repair        # 安装boot-repair
sudo boot-repair              # 在打开的图形化界面中选择Recommended repair进行修复

### 2.6 重启系统

reboot
重启完成后,系统就已经迁移过来了.这时可以删除/restore

## 优化上面的方法

## 压缩时直接排除这两个文件

sudo tar -czvpf backup.tar.gz  --exclude=/proc --exclude=/sys --exclude=/run --exclude=/mnt --exclude=/tmp --exclude=/media --exclude=/lost+found --exclude=/etc/fstab --exclude=/boot/grub/grub.cfg /

## 在根目录/下创建文件夹,并将原有系统所在的磁盘分区挂载到该文件夹上

mkdir restore      # 创建临时挂载文件
df -h        # 查看分区,找出根目录/ 所挂载的磁盘分区
mount /dev/sdaX /restore   # 假定原有系统在磁盘分区/dev/sdaX上

## 解压

sudo tar -xzvpf backup.tar.gz -C /restore

## 引导修复

sudo boot-repair

/************************************ 不保存直接修改配置文件(这种方法我没试验成功) *****************************************/
系统启动文件配置
把新硬盘的/boot/grub/grup.cfg里面的uuid替换掉
如果没有把boot单独分区,那么只要将里面的uuid全部改成新硬盘的uuid,(考虑启动过程没有其他挂载的情况)。如果是单独分区的话,注意这个文件下还有boot的uuid的值(我的硬盘没有boot单独分区)。
uuid查询方法:

1. sudo blkid
2. ls -l /dev/disk/by-uuid
sudo vim /boot/grub/grub.cfg

%s /uuid_old/uuid_new/g

分区挂载配置
修改 /etc/fstab 文件，把要启动自动挂载的分区都写上
sudo vim /etc/fstab

%s /uuid_root_old/uuid_root_new/g
%s /uuid_home_old/uuid_home_new/g

or

genfstab -U /sda1 >> /mnt/etc/fstab
genfstab -U /sda2 >> /mnt/etc/fstab
/************************************ 不保存直接修改配置文件 *****************************************/
