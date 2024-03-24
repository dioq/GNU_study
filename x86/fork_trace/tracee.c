#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

void *thread_function(void *arg);
char message[] = "hello";
int main()
{
    int res;
    pthread_t a_thread;
    void *thread_result;
    res = pthread_create(&a_thread, NULL, thread_function, (void *)message);
    if (res != 0)
    {
        printf("Thread creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Wait for thread to finish...\n");
    res = pthread_join(a_thread, &thread_result);
    if (res != 0)
    {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }
    printf("Thread joined, it returned %s\n", (char *)thread_result);
    exit(EXIT_SUCCESS);
    return 0;
}
void *thread_function(void *arg)
{
    printf("thread_function is running ,Argument was %s\n", (char *)arg);
    pthread_exit("Thank you for the CPU time");
}
