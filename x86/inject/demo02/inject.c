//#include <stdio.h>

int func0(){
  //      printf("************** this is in so library ...\n");
	return 10;
}

int func1(int p1) {
	return (p1 + 10);
}

int func2(int p1, int p2) {
	return (p1 + p2);
}

void read_mem(unsigned long vaddr,unsigned long len) {
    //    printf("Hello, this is read_mem ...\n");
        //uint8_t * tmp_addr = (uint8_t *)vaddr;
        //for(unsigned long i = 0; i < len; i++) {
        //        printf("%02x ",*(tmp_addr + i));
        //}
        //puts("");
}

