cc=gcc

test:
	$(cc) -no-pie anti_trace.c -o tracee
	./tracee

program:
	$(cc) -no-pie ptrace_attach01.c -o main
	./main `pidof ./tracee`
