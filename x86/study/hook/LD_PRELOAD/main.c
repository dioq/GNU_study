#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if(strcmp(argv[1], "test"))
	{
		printf("Incorrect password\n");
	}
	else
	{
		printf("Correct password\n");
	}

	return 0;
}
