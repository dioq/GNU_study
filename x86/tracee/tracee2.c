#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdint.h>

int func10(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10) {
	long a1 = 10;
	long a2 = 11;
	long a3 = 12;
	long a4 = 13;
	printf("Hello, this is func10 ...\n");
	printf("p1 = %d,p2 = %d,p3 = %d,p4 = %d,p5 = %d,p6 = %d,p7 = %d,p8 = %d,p9 = %d,p10 = %d\n",
			p1,p2,p3,p4,p5,p6,p7,p8,p9,p10);
	long tmp = a1 + a2 + a3 + a3;
	return tmp + p1+p2+p3+p4+p5+p6+p7+p8+p9+p10;
}

/*
 * gcc -no-pie tracee.c -o tracee
 * **/
int main() {
	int p1 = 0x1;
	int p2 = 0x2;
	int p3 = 0x3;
	int p4 = 0x4;
	int p5 = 0x5;
	int p6 = 0x6;
	int p7 = 0x7;
	int p8 = 0x8;
	int p9 = 0x9;
	int p10 = 0x10;
	
	int ret = func10(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10);
	
	return 0;
}
