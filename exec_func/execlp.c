#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*
 帶 p 的exec函數：execlp,execvp，表示第一個參數path不用輸入完整路徑，只給出命令名即可，它會在環境變量PATH當中查找命令
 * */
int main(void){
    printf("entering main process---\n");
    // 带p 只用给出命名即可
    execlp("ls","ls","-l",NULL);
    // 不带p 给出全路径
    //execl("/bin/ls","ls","-l",NULL);
    printf("exiting main process ----\n");
    return 0;
}
