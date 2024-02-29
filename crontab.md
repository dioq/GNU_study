# crontab 定时任务

crontab命令是cron table的简写，它是cron的配置文件，也可以叫它作业列表。

/var/spool/cron/        目录下存放的是每个用户包括root的crontab任务，每个任务以创建者的名字命名
/etc/crontab            这个文件负责调度各种管理和维护任务。
/etc/cron.d/            这个目录用来存放任何要执行的crontab文件或脚本。

/etc/cron.hourly            每小时执行一次
/etc/cron.daily             每天时执行一次
/etc/cron.weekly            每周时执行一次
/etc/cron.monthly           每月时执行一次

## 自定义任务

命令格式
crontab [-u user] file crontab [-u user] [ -e | -l | -r ]

命令参数
-u user：用来设定某个用户的crontab服务；
file：file是命令文件的名字,表示将file做为crontab的任务列表文件并载入crontab。如果在命令行中没有指定这个文件，crontab命令将接受标准输入（键盘）上键入的命令，并将它们载入crontab。
-e：编辑某个用户的crontab文件内容。如果不指定用户，则表示编辑当前用户的crontab文件。
-l：显示某个用户的crontab文件内容，如果不指定用户，则表示显示当前用户的crontab文件内容。
-r：从/var/spool/cron目录中删除某个用户的crontab文件，如果不指定用户，则默认删除当前用户的crontab文件。
-i：在删除用户的crontab文件时给确认提示。

crontab -e      编译定时任务
crontab -l      列出所有定时任务

## crontab 文件格式

分 时 日 月 星期 要运行的命令
第1列分钟0～59
第2列小时0～23（0表示子夜）
第3列日1～31
第4列月1～12
第5列星期0～7（0和7表示星期天）
第6列要运行的命令

每分钟执行一次
`* * * * * /bin/bash /home/zd/test/test.sh`
每年 1,3,6,9,12 月份的 1 日 0 时 0分 执行1次
`0 0 1 1,3,6,9,12 * certbot renew`
每天 0 时 0分 执行1次
0 0 1-31 * * kill -9 `pidof python3`
