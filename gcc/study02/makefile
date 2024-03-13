cc=gcc

# 先编译一个静态库
static_library:
	${cc} -c add.c -o add.o
	${cc} -c sub.c -o sub.o
	${cc} -c mul.c -o mul.o
	${cc} -c div.c -o div.o
	ar -cr libmymath.a add.o sub.o mul.o div.o

dynamic_library: static_library
	 ${cc} -c func.c -o func.o
	 ${cc} -shared -fPIC func.o libmymath.a -o libmymath.so

program: dynamic_library
	${cc} main.c -o main -L./ -lmymath

# 编译的时候使用 -Wl,-rpath=/tmp/ 指定运行时依赖库查找路径为/tmp/
program1: dynamic_library
	${cc} -Wl,-rpath=/tmp/ main.c -o main -L./ -lmymath
	@mv libmymath.so /tmp/

all: program1
	./main

clean:
	@-rm -rf *.o *.so *.a main
