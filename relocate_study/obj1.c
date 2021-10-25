#include <stdio.h>
#include "./obj2.h"

int main(int regc,char * regv[]){
 int ret = foo(3,4);
 printf("ret = %d\n",ret);
 return 0;
}
