#define _GNU_SOURCE
#include <link.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

#define PAGE_SHIFT 12
#define PAGE_SIZE   (1UL << PAGE_SHIFT)
#define PAGE_MASK   (~(PAGE_SIZE-1))

#define PAGE_START(addr) ((addr) & PAGE_MASK)
#define PAGE_END(addr)   (PAGE_START(addr) + PAGE_SIZE)

uintptr_t base_addr;

// 查看所有加载的so
static int callback(struct dl_phdr_info *info, size_t size, void *data) {
	char *type;
	int p_type, j;
	if (!strcmp(info->dlpi_name, "./libtest.so")) {
		printf("dlpi_name : %s , base addr = %p\n",info->dlpi_name, (void *)info->dlpi_addr);
		base_addr = info->dlpi_addr;
		printf("base_addr = %p\n",(void *)base_addr);
	}
	//    printf("Name: \"%s\" (%d segments)\n", info->dlpi_name, info->dlpi_phnum);

	//    for (j = 0; j < info->dlpi_phnum; j++) {
	//        p_type = info->dlpi_phdr[j].p_type;
	//        type =  (p_type == PT_LOAD) ? "PT_LOAD" :
	//                (p_type == PT_DYNAMIC) ? "PT_DYNAMIC" :
	//                (p_type == PT_INTERP) ? "PT_INTERP" :
	//                (p_type == PT_NOTE) ? "PT_NOTE" :
	//                (p_type == PT_INTERP) ? "PT_INTERP" :
	//                (p_type == PT_PHDR) ? "PT_PHDR" :
	//                (p_type == PT_TLS) ? "PT_TLS" :
	//                (p_type == PT_GNU_EH_FRAME) ? "PT_GNU_EH_FRAME" :
	//                (p_type == PT_GNU_STACK) ? "PT_GNU_STACK" :
	//                (p_type == PT_GNU_RELRO) ? "PT_GNU_RELRO" : NULL;
	//
	//       // printf("    %2d: [%14p; memsz:%7lx] flags: 0x%x; ", j,
	//       //         (void *) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr),
	//       //         info->dlpi_phdr[j].p_memsz,
	//       //         info->dlpi_phdr[j].p_flags);
	//       printf("dlpi_name : %s , base addr = %p\n",info->dlpi_name, (void *)info->dlpi_addr);
	//        if (type != NULL)
	//            printf("%s\n", type);
	//        else
	//            printf("[other (0x%x)]\n", p_type);
	//    }

	return 0;
}

void *my_malloc(size_t size)
{
	printf("%zu bytes memory are allocated by libtest.so\n", size);
	return malloc(size);
}

void hook() {
	char       line[512];
	uintptr_t  addr;


	if(0 == base_addr) return;
	printf("base : %p\n", (void *)base_addr);

	//the absolute address
	addr = base_addr + 0x1119;
	printf("addr : %p\n", (void *)addr);

	//add write permission
	void *page_addr = (void *)PAGE_START(addr);
	printf("page_addr : %p\n", page_addr);
	int err = mprotect(page_addr, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
	//int err = mprotect(addr, PAGE_SIZE, PROT_READ | PROT_WRITE);
	if (err < 0) {
		perror("mprotect err :");
		exit(-1);
	}
	printf("line : %d\n",__LINE__);
	//replace the function address
//	return;
	*(void **)addr = my_malloc;
	//memset(addr,'a',10);
	printf("line : %d\n",__LINE__);

	//clear instruction cache
	__builtin___clear_cache((void *)PAGE_START(addr), (void *)PAGE_END(addr));
}


int main() {
	dl_iterate_phdr(callback, NULL);
	hook();
	hello();

	//printf("base : %p\n", base_addr);

	return 0;
}

