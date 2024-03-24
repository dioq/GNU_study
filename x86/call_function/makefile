cc = gcc

exec = main

source = vaddr_by_symbol.c ptrace_util.c call05.c

all: $(exec)
	@./$(exec) `pidof ./tracee`

$(exec):
	@$(cc) -no-pie $(source) -o $(exec) -ldl

so: inject.c
	@$(cc) -fPIC -shared inject.c -o libinject.so

clean:
	@-rm $(exec)
	clear
