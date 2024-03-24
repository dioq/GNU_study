cc = gcc

.so = libcaculate.so
exec = tracee

all: $(exec)
	@ ./$(exec)

$(exec): tracee.c
	@ $(cc) -no-pie tracee.c -o $(exec) -ldl

so : caculate.c
	@ $(cc) -fPIC -shared caculate.c -o $(.so)

.PHONY:clean
clean:
	@-rm $(exec) $(.so)
