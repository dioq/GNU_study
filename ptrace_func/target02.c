#include<unistd.h>
#include <stdio.h>
int main()
{    
    printf("pid=%d\n",getpid());
    for(int num=0;num<20;num++)
    {
    printf("num = %d\n",num);
    sleep(2);
    }
    return 0;
}
