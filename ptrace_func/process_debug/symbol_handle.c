#include <symbol_handle.h>

Elf64_Addr lookup_symbol(handle_t *h, const char *symname){
	int i, j;
	char *strtab;
	Elf64_Sym *symtab;
	for (i = 0; i < h->ehdr->e_shnum; i++)
	{
		if (h->shdr[i].sh_type == SHT_SYMTAB)
		{
			strtab = (char *)&h->mem[h->shdr[h->shdr[i].sh_link].sh_offset];
			symtab = (Elf64_Sym *)&h->mem[h->shdr[i].sh_offset];
			for (j = 0; j<h>
					shdr[i]
					.sh_size /
					sizeof(Elf64_Sym);
					j++)
			{
				if (strcmp(&strtab[symtab->st_name], symname) == 0)
					return (symtab->st_value);
				symtab++;
			}
		}
	}
	return 0;
}
