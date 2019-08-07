#include <stdio.h>
#include <stdlib.h>

typedef struct test_struct
{
	long num;
	char *p;
}test_struct;

int main(void)
{
	test_struct *ptr = NULL;

	
	ptr = (test_struct *)calloc(2, sizeof(test_struct));
	
	printf("%ld %ld\n", ptr[0].num, ptr[1].num);

	if(0L == ptr[0].num)
	{
		printf("calloc init long to 0 \n");
	}
	
	if(NULL == ptr[0].p)
	{
		printf("calloc init pointer to NULL \n");
	}
	
	
	free(ptr);
	return 0;
}