#include <stdio.h>
#include <unistd.h>

void print_string(long *num){
	printf("current pid = %d\t",getpid());
	printf("Restart a new life. %ld\n",*num);
	(*num)++;
}

int main() {
	long num = 0;
	while(1){
		print_string(&num);
		sleep(2);
	}
	return 0;
}
