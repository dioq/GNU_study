# 磁盘

## 磁盘操作

lsblk           查看当前磁盘情况
df -lh          查看文件系统情况 -l 查看挂载点
parted -l       会列出文件系统类型
fdisk -l        查看当前未挂载硬盘

## 挂载外接存储设备

1. 将外接存储设备(我这里用的是u盘)连接到操作系统上
/sbin/fdisk -l
可以查看到外接设备所在的分区(我这里是 /dev/sdb)
2. 挂载
mkdir /mnt/usb  # 创建挂载文件夹(我这里命名usb)
mount /dev/sdb /mnt/usb
挂载完成后 就可以进入 /mnt/usb 目录里查看u盘里的数据
3. 取消挂载
umount /mnt/usb
当不再需要连接u盘时,umount可以取消刚才的挂载操作
