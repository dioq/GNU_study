#include "ptrace_util.h"

int push_stack(int pid, unsigned long *sp, long * paramers,int len) {
	const unsigned int MAX_STACK = 80000;
	union {
		long val[MAX_STACK];
		uint8_t bytes[MAX_STACK * LONGSIZE];
	} data;
 
	unsigned long statck_delta = len * LONGSIZE;
	//printf("push *sp = 0x%lx\n", *sp);
	*sp -= statck_delta;
	//printf("push *sp = 0x%lx\n", *sp);

	for (unsigned int i = 0; i < len; i++) {
		data.val[len - 1 - i] = paramers[i];
	}

	putdata(pid, *sp, data.bytes, statck_delta);

//	printf("push new *sp = 0x%lx\n", *sp);
//	uint8_t *tmp = (uint8_t *)calloc(num_param,8);
//        //getdata(pid, *sp, data.bytes, statck_delta);
//        getdata(pid, *sp, tmp, 32);
//        for(int i = 0; i< 32; i++) {
//                printf("%02x ", tmp[i]);
//        }
//        puts("\n******* push  ********");

	return 0;
}

int pop_stack(int pid, unsigned long *sp, long *paramers, int len) {
	const unsigned int MAX_STACK = 80000;
	union {
		long val[MAX_STACK];
		uint8_t bytes[MAX_STACK * LONGSIZE];
	} data;

	unsigned long statck_delta = len * LONGSIZE;
	
	//printf("pop new *sp = 0x%lx\n", *sp);
	getdata(pid, *sp, data.bytes, statck_delta);
	
//	uint8_t *tmp = (uint8_t *)calloc(num_param,8);
//	getdata(pid, *sp, tmp, 32);
//	for(int i = 0; i< 32; i++) {
//		printf("%02x ", tmp[i]);
//	}
//	puts("\n********* pop **********");
	
	for (int i = 0; i < len; i++) {
		paramers[i] = data.val[i];
	}

	*sp += statck_delta;

	//printf("pop *sp = 0x%lx\n", *sp);
	return 0;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage:\n\t%s pid\n", argv[0]);
		return -1;
	}
	pid_t target_pid = atoi(argv[1]);

	ptrace_attach(target_pid);
	int status;
	wait(&status);

	long data[] = {0x20,0x30,0x40,0x50,0x60,0x70,0x80};
	const int len = sizeof(data) / sizeof(long);
	
	uint64_t rsp = 0x10e02a0 + 0x40;
	printf("%d rsp = 0x%lx\n",__LINE__ ,rsp);
	push_stack(target_pid, &rsp, data, len);
	
	long pop_data[256];
	pop_stack(target_pid, &rsp, pop_data, len);

	printf("pop data:\n");
	for(int i = 0; i< len; i++) {
		printf("%lx\t",pop_data[i]);
	}
	puts("");

	//	ptrace_cont(target_pid);

	return 0;
}
