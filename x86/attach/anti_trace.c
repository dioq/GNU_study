#include <stdio.h>
#include <unistd.h>
#include <sys/ptrace.h>

void print_string(long *num)
{
	printf("current pid = %d\t", getpid());
	printf("Restart a new life. %ld\n", *num);
	(*num)++;
}

int main()
{
	printf("current pid = %d,", getpid());
	// 这里就直接先执行了ptrace(PTRACE_TRACEME, 0, 0, 0)，表示此程序已经被追踪
	int ret = ptrace(PTRACE_TRACEME, 0, 0, 0);
	printf("ptrace ret is %d\n", ret);
	if (ret == -1)
	{
		printf("don't trace me!\n");
		return 1;
	}
	printf("no one trace me!\n");

	long num = 0;
	while (1)
	{
		print_string(&num);
		sleep(2);
	}

	return 0;
}
