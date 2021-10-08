# exec系列函数

exec 系列函数（execl、execlp、execle、execv、execvp）
原型:
int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *path, const char *arg, ..., char * const envp[]);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
參數：
path參數表示你要啟動程序的名稱包括路徑名
arg參數表示啟動程序所帶的參數，一般第一個參數為要執行命令名，不是帶路徑且arg必須以NULL結束
返回值:成功返回0,失敗返回-1
注：上述exec系列函數底層都是通過execve系統調用實現：
#include <unistd.h>
int execve(const char *filename, char *const argv[],char *const envp[]);

exec替换進程映像 在進程的創建上Unix採用了一個獨特的方法，它將進程創建與加載一個新進程映象分離。這樣的好處是有更多的餘地對兩種操作進行管理。
當我們創建了一個進程之後，通常將子進程替換成新的進程映象，這可以用exec系列的函數來進行。當然，exec系列的函數也可以將當前進程替換掉。
例如：在shell命令行執行ps命令，实际上是shell進程調用fork複製一個新的子進程，在利用exec系統調用將新產生的子進程完全替換成ps進程。


1，帶 l 的exec函數：execl,execlp,execle，表示後邊的參數以可变参数的形式給出且都以一個空指針結束。
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(void) {
    printf("entering main process---\n");
    execl("/bin/ls","ls","-l",NULL);
    printf("exiting main process ----\n");
    return 0;
}

2，帶 p 的exec函數：execlp,execvp，表示第一個參數path不用輸入完整路徑，只要給出命令名即可，它會在環境變量PATH當中查找命令
带p 只用给出命名即可:
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(void){
    printf("entering main process---\n");
    execlp("ls","ls","-l",NULL);
    printf("exiting main process ----\n");
    return 0;
}

3，不帶 l 的exec函數：execv,execvp表示命令所需的參數以char *arg[]形式給出且arg最後一個元素必須是NULL
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(void) {
    printf("entering main process---\n");
    int ret;
    char *argv[] = {"ls","-l",NULL};
    ret = execvp("ls",argv);
    if(ret == -1)
        perror("execl error");
    printf("exiting main process ----\n");
    return 0;
}

4，帶 e 的exec函數：execle表示，將环境变量传递給需要替換的进程
environ保存環境信息的數據可以env命令查看：
$root: env
SHELL=/bin/bash
SESSION_MANAGER=local/dvm:@/tmp/.ICE-unix/1616,unix/dvm:/tmp/.ICE-unix/1616
QT_ACCESSIBILITY=1
COLORTERM=truecolor
XDG_CONFIG_DIRS=/etc/xdg/xdg-ubuntu:/etc/xdg
....

它由shell進程传递給當前進程，再由當前進程传递給替換的新進程
示例：execl.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
    printf("Entering main ...\n");
    int ret;
    ret =execl("./bin/target", "param 1","param 2", NULL);
    if(ret == -1)
        perror("execl error");
    printf("Exiting main ...\n");
    return 0;
}
运行后：
运行后会发现原进程的环境变量信息传递给了新进程

那么现在我們可以利用execle函數传递自己定义的环境变量信息：
示例程序：execle.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
    char * const envp[] = {"AA=11", "BB=22", NULL};
    printf("Entering main ...\n");
    int ret;
    //ret =execl("./bin/target", "param 1","param 2", NULL);
    // 利用execle函數传递自己定义的环境变量信息：
    ret =execle("./bin/target", "param 1","param 2", NULL, envp);
    if(ret == -1)
        perror("execl error");
    printf("Exiting main ...\n");
    return 0;
}
运行后
确实將自己定义的环境变量传递过来了

用于接受传递的目标函数
示例程序：target.c
#include <stdio.h>
#include <unistd.h>
// 此处的environ是一個指針數組，它當中的每一個指針指向的char为"XXX=XXX"
extern char** environ;
int main(int argc,char * argv[]) {
    printf("argv[0] = %s, argv[1] = %s\n",argv[0],argv[1]);
    printf("target pid=%d\n", getpid());
    int i;
    for (i=0; environ[i]!=NULL; ++i)
    {
        printf("%s\n", environ[i]);
    }
    return 0;
}