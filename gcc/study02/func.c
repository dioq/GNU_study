#include "func.h"

int funcX(int a, int b)
{
    int tmp = add(a, b);
    return tmp * b;
}

int funcY(int a, int b) {
    int tmp = sub(a, b);
    return tmp * b;
}