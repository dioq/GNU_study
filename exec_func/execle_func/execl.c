#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// 运行后会发现原进程的环境变量信息传递给了新进程
int main(int argc, char *argv[]) {
    //char * const envp[] = {"AA=11", "BB=22", NULL};
    printf("Entering main ...\n");
    int ret;
    ret =execl("./bin/target", "param 1","param 2", NULL);
    //execle("./hello", "hello", NULL, envp);
    if(ret == -1)
        perror("execl error");
    printf("Exiting main ...\n");
    return 0;
}
