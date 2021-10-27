#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * 不帶 l 的exec函數：execv,execvp表示命令所需的參數以char *argv[]形式給出且argv最後一個元素必須是NULL
 * */
int main(void)
{
    printf("entering main process---\n");
    int ret;
    char *argv[] = {"ls","-l",NULL};
    ret = execvp("ls",argv);
    if(ret == -1)
        perror("execl error");
    printf("exiting main process ----\n");
    return 0;
}
