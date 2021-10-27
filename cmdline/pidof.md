对于基于Red Hat和Debian的发行版，pidof有不同的实现。 在Red Hat发行版中，pidof命令是procps-ng软件包的一部分，而在Debian上，它是sysvinit-utils的一部分。 我们将介绍两种实现方式共有的选项。


pidof命令的语法如下：
pidof [OPTIONS] PROGRAM_NAME
该命令接受零个或多个名称作为参数，但是通常，您只将一个名称传递给pidof。

不带任何选项调用时，pidof将打印与给定名称匹配的所有正在运行程序的PID。 例如，要找到SSH服务器的PID，可以运行：
pidof sshd

如果有名称与sshd匹配的正在运行的进程，则其PID将显示在屏幕上。 如果未找到匹配项，则输出为空。

4382 4368 811
当至少一个正在运行的程序与请求的名称匹配时，pidof返回0。 否则，退出代码为1。 这在编写shell脚本时很有用。

为确保仅显示要搜索的程序的PID，请使用程序的完整路径名作为参数。 例如，如果您有两个名称相同的正在运行的程序位于两个不同的目录中，则pidof将显示两个正在运行的程序的PID。

默认情况下，显示匹配的运行程序的所有PID。 使用-s选项强制pidof仅显示一个PID：

pidof -s program_name
-o选项可让您从命令输出中排除具有指定PID的进程：

pidof -o pid program_name
当通过-o选项调用pidof时，可以使用名为%PPID的特殊PID来表示调用shell程序或shell脚本。要仅返回使用相同根目录运行的进程的PID，请使用-c选项。

此选项仅在以root用户或sudo用户身份运行pidof时起作用：

pidof -c pid program_name
pidof命令的用法示例
以下示例显示如何结合使用pidof命令和kill命令来终止程序。假设Firefox浏览器已无响应，因此您需要终止Firefox进程。 首先用pidof找到PID。

pidof firefox
该命令将打印所有Firefox进程：

2551 2514 1963 1856 1771
一旦您知道Firefox进程的PID，请发送SEGTERM信号以终止所有PID：

sudo kill -9 2551 2514 1963 1856 1771
您还可以使用命令替换表达式$(...)在一个命令中终止程序：

sudo kill -9 $(pidof firefox)

