#include <sys/ptrace.h>
#include <stdio.h>
#include <unistd.h>
int main() {
   printf("current pid = %d\n",getpid());
   //这里就直接先执行了ptrace(PTRACE_TRACEME, 0, 0, 0)，表示此程序已经被追踪
   int ret = ptrace(PTRACE_TRACEME, 0, 0, 0);
   printf("ptrace ret is %d\n",ret);   
   if (ret == -1) {
        printf("don't trace me！\n");
        return 1;
    }
    printf("no one trace me！\n");
    return 0;
}
