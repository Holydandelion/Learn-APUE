

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	int count = 300000000;
	double num = 0.1;
	struct timespec ts_before;
	struct timespec ts_after;
	
	if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_before) == -1)
		printf("[ERROR]clock_gettime error:%s \n", strerror(errno));
	
	while(count--)
		num += 0.1;
	
	if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_after) == -1)
		printf("[ERROR]clock_gettime error:%s \n", strerror(errno));
		
	printf("[DEBUG]ts_before.tv_sec:%ld  ts_before.tv_nsec:%ld \n", ts_before.tv_sec, ts_before.tv_nsec);
	printf("[DEBUG]ts_after.tv_sec:%ld  ts_after.tv_nsec:%ld \n", ts_after.tv_sec, ts_after.tv_nsec);
	printf("[DEBUG]total:tv_sec:%ld  tv_nsec:%ld \n", ts_after.tv_sec - ts_before.tv_sec, ts_after.tv_nsec - ts_before.tv_nsec);	
	
	exit(EXIT_SUCCESS);
}