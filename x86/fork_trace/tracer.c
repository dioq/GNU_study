#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    if (argc < 2)
    {
        fprintf(stderr, "Usage:\n\t%s pid\n", argv[0]);
        exit(1);
    }

    pid_t target_pid = atoi(argv[1]);
    long ptraceOption = PTRACE_O_TRACECLONE;
    ptrace(PTRACE_SETOPTIONS, target_pid, NULL, ptraceOption); // 设置ptrace属性

    int status;
    pid_t child_waited = waitpid(-1, &status, WUNTRACED); // 等待接收信号

    if (WIFSTOPPED(status))
    {
        printf("child %ld recvied signal %d\n", (long)child_waited, WSTOPSIG(status));
    }
    if (WIFSIGNALED(status))
    {
        printf("child %ld recvied signal %d\n", (long)child_waited, WTERMSIG(status));
    }

    if (WIFEXITED(status))
    { // 线程结束时，收到的信号
        printf("thread %d exited with status %d\t\n", child_waited,
               WEXITSTATUS(status));
    }
    ptrace(PTRACE_CONT, child_waited, 1, NULL);

    return 0;
}
