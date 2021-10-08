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
