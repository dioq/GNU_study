#include <stdio.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    pid_t child;
    long orig_rax;
 
    child = fork();
 
    if(child == 0){
	printf("child pid is %d\n",getpid());
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);  // Tell kernel, trace me
        execl("../bin/anti_ptrace","param",NULL);
    }else{  
        /*Receive certification after child process stopped*/
        wait(NULL);
	printf("father pid %d is over\n",getpid());
   }
 
    return 0;
}
