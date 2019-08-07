

#include <sys/select.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void sleep_us(int us);
int main(void)
{
	struct timespec time_before, time_after;
	clock_gettime(CLOCK_REALTIME, &time_before);
	sleep_us(1000);
	clock_gettime(CLOCK_REALTIME, &time_after);	
	printf("time eclpsed: %ld s, %ld ns\n", time_after.tv_sec - time_before.tv_sec, time_after.tv_nsec - time_before.tv_nsec);
	exit(0);
}

void sleep_us(int us)
{
	struct timeval time_value;
	time_value.tv_sec = us/1000000;
	time_value.tv_usec = us%1000000;
	select(0, NULL, NULL, NULL, &time_value);
}
