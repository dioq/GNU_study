# Linux kernel

## x86 获取保存寄存器信息的结构体

在i386结构下包含的头文件是<linux/user.h>
在x64机器上需要进行修改,改成<sys/user.h>
其实最终访问的文件是/usr/include/x86_64-linux-gnu/sys/user.h

eip寄存器存储着我们cpu要读取指令的地址,没有了它,cpu就无法读取下面的指令
RIP --- x64体系
EIP --- x86体系

struct user_regs_struct regs;

## 检测是否处于被调试状态

通过GET_THREAD_INFO宏获得当前进程的thread_info结构的地址
再检测当前进程是否被其他进程所跟踪(例如调试一个程序时,被调试的程序就处于被跟踪状态),也就是thread_info结构中flag字段的_TIF_ALLWORK_MASK被置1。
如果发生被跟踪的情况则转向syscall_trace_entry标记的处理命令处。
