#include <stdio.h>
#include <string.h>

// gcc target.c -o target
int main(int argc, char *argv[]) {
	if(!strcmp(argv[1], "123")) {
		printf("Incorrect password\n");
	} else {
		printf("Correct password\n");
	}

	return 0;
}
