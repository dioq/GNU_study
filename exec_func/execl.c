#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* 
  帶 l 的exec函數：execl,execlp,execle，表示後邊的參數以可变参数的形式給出且都以一個空指針結束
 
  利用execl將當前進程main替換掉，所有最後那條打印語句不會輸出
*/
int main(void)
{
    printf("entering main process pid = %d ---\n",getpid());
    execl("/bin/ls","ls","-l",NULL);
    printf("exiting main process ----\n");
    return 0;
}
