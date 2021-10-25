gcc tracer.c -o tracer

./tracer -p `pidof ./test2` -f print_string
