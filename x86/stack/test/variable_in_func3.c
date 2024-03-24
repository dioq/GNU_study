#include <stdio.h>

long random(long p1,long p2, long p3) {
	long tmp = p1 + p2 + p3;
	printf("tmp = %ld\n",tmp);
	long ret = tmp % 10;
	return ret;
}

int main() {
	long a1 = 0x1;
	long a2 = 0x2;
	long a3 = 0x3;
	long a4 = 0x4;
	long a5 = 0x5;
	long a6 = 0x6;
	long a7 = 0x7;
	long a8 = 0x8;
	long a9 = 0x9;
//	long b1 = 0xa;
//	long b2 = 0xb;
//	long b3 = 0xc;
//	long b4 = 0xd;
//	long b5 = 0xe;
//	long b6 = 0xf;
	long rand_num = random(a1,a2,a3);
	//long total = a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9 + b1 + b2;
	printf("rand_num = %ld\n", rand_num);

	return 0;
}
