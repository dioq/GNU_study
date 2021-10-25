#include <stdio.h>
#include <unistd.h>

void print_string(){
	printf("hello new life\n");
}

int main() {
	while(1){
		print_string();
		sleep(1);
	}
	return 0;
}
