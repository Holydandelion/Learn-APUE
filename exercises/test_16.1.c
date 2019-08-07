
#include <stdio.h>
#include <stdlib.h>

typedef union endian_test
{
	int i;
	char c[sizeof(int)];
}endian_test;

int main(void)
{
	endian_test endian_un;
	endian_un.i = 0x04030201;
	
	if (endian_un.c[0] == 0x01)
		printf("Little endian\n");
	else if (endian_un.c[0] == 0x04)
		printf("Big endian.\n");
	else
		printf("Unknown.\n");
	
	exit(0);
}