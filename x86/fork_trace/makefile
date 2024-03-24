cc = gcc

tracee: tracee.c
	$(cc) tracee.c -o tracee -lpthread

tracer: tracer.c
	$(cc) tracer.c -o tracer -lpthread


all: tracee tracer

clean:
	@- rm -rf tracee tracer

