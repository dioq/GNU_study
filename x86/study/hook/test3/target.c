#include <stdio.h>
#include <unistd.h>

void func11() {
	printf("1111 function  ----------- end \n");
}

void func22() {
	printf("2222 function  ----------- end\n");
}

void print_addr() {
    void *func11_addr = NULL;
    //*(void **)target_addr = target_func;
    func11_addr = func11;
    printf("func11_addr :       %p\n", func11_addr);

    void *func22_addr = NULL;
    func22_addr = func22;
    printf("func22_addr : %p\n", func22_addr);
}

int main() {
	print_addr();
	//hook();
	while(1) {
		func11();
		func22();
		sleep(5);
		puts("-----------------------------");
	}
	return 0;
}
