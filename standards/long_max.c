

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
 	long long ll = (1ULL << (sizeof(long long)*8 - 1)) - 1;
	printf("signed long long is %ld \n", ll);
	exit(0);
}