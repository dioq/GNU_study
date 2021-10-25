#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	printf("Entering main current pid = %d ...\n",getpid());
	/*
         * param1 param2 传递给 process image(./bin/target) 里 main 函数的 argv[]
         * 但是同是默认把原进程的环境变量信息传递给了新进程
         * ./bin/target 里的 extern char** environ; 保存的就是原进程的环境变量信息
         */
	int ret = execl("./bin/target", "param 1","param 2", NULL);
	if(ret == -1)
		perror("execl error");
	printf("Exiting main ...\n");
	return 0;
}
