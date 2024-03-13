cc=gcc

# 先编译一个静态库
static_library:
	${cc} -c add.c -o add.o
	${cc} -c sub.c -o sub.o
	${cc} -c mul.c -o mul.o
	${cc} -c div.c -o div.o
	ar -cr libmymath.a add.o sub.o mul.o div.o


# 编译的动态库需要引用另一个静态库
# 将func.o 与 libmymath.a 里的 add.o sub.o 链接成一个动态库, 编译器会自动根据 func.o 找到的相关可重定位 object file (add.o sub.o) 
# 然后链接成一个动态库。libmymath.a 里的无关 object file (mul.o div.o) 不会被链接到生成的动态库里
dynamic_library: static_library
	 ${cc} -c func.c -o func.o
	 ${cc} -shared -fPIC func.o libmymath.a -o libmymath.so

# 与 dynamic_library 完全等价
dynamic_library1: static_library
	 ${cc} -c func.c -o func.o
	 ${cc} -shared -fPIC func.o add.o sub.o -o libmymath.so

# 与 dynamic_library 相比把 mul.o div.o 也编译到动态库里了
dynamic_library2: static_library
	 ${cc} -c func.c -o func.o
	 ${cc} -shared -fPIC func.o add.o sub.o mul.o div.o -o libmymath.so

# -L 指定依赖库查找的位置，注意L后面还有'./',表示在当前目录下查找
# -l 指定依赖库名，其中的lib和.a(.so)省略。
main: dynamic_library2
	${cc} main.c -o main -L./ -lmymath

test10:
	${cc} -o main10 main.c func.c add.c sub.c mul.c

test11: static_library
	${cc} -c func.c -o func.o
	${cc} -c main.c -o main.o
	${cc} -o main11 main.o func.o add.o sub.o mul.o

test20:
	${cc} -o main20 main.c func.c add.c sub.c

test21: static_library
	${cc} -c func.c -o func.o
	${cc} -c main.c -o main.o
	${cc} -o main21 main.o func.o add.o sub.o

all: main
# 环境变量LD_LIBRARY_PATH指定动态库搜索路径(在 shell 里执行)
	export LD_LIBRARY_PATH=./
# 默认的动态库搜索路径 /lib  /usr/lib
#	 sudo mv libmymath.so /usr/lib
	./main


clean:
	@-rm -rf *.o *.so *.a
	@-rm -rf main main10 main11 main20 main21
	@-sudo rm -rf /usr/lib/libmymath.so
