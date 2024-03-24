//#include <stdio.h>

typedef int(*MY_PRINTF)(const char *__restrict __format, ...);

int init_symbol(void *printf_vaddr) {
	MY_PRINTF my_print = 0;
	my_print = (MY_PRINTF) printf_vaddr;
	char *str = "Hello world! init_symbol";
	//char *str = (char *)0x401258;
	my_print("%s\n", str);
	return 0;
}

//int call_printf() {
//    MY_PRINTF my_print = 0;
//    my_print = (MY_PRINTF) printf_vaddr;
//    char *str = "Hello world!";
//    my_print("%s\n", str);
//
//    return 0;
//}

int func0(){
  //      printf("************** this is in so library ...\n");
	return 10;
}

int func1(int p1) {
	return (p1 + 10);
}

int func2(int p1, int p2) {
//	char *str = "Hello world!";
//	my_print("%s\n", str);
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


//typedef int(*CALL_FUNC)(int a, int b);
//int call_func(void *func_addr,int p1, int p2) {
//	CALL_FUNC func = 0;
//	func = (CALL_FUNC)func_addr;
//	
//	func(p1,p2);
//
//	return 0;
//}
