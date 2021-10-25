#include <elf.h>

typedef struct handle {
  Elf64_Ehdr *ehdr;
  Elf64_Phdr *phdr;
  Elf64_Shdr *shdr;
  uint8_t    *mem;
  char       *symname;
  Elf64_Addr  symaddr;
  struct user_regs_struct pt_reg;
  char       *exec;
} handle_t;
