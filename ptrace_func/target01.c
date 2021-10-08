#include <stdio.h>
int main(){
    for(int i = 0;i < 100; i++) {
        printf("My counter: %d\n", i);
        sleep(2);
    }
    return 0;
}
