/*
   生成动态链接库

   gcc -fPIC -shared caculate.c -o libcaculate.so
 * */

char *add_str = "add";
char *sub_str = "sub";
char *mul_str = "mul";
char *div_str = "div";

int add(int a,int b) {
	return (a + b);
}

int sub(int a, int b) {
	return (a - b);
}

int mul(int a, int b) {
	return (a * b);
}

int div(int a, int b) {
	return (a / b);
}
