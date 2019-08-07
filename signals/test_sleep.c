


#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


static void print_time(const  char* str);


int main(void)
{
	struct timespec time_spec, old_time_spec;
	
	memset(&time_spec, 0x00, sizeof(time_spec));
	memset(&old_time_spec, 0x00, sizeof(old_time_spec));
		
	print_time("before sleep");	
	sleep(3);
	print_time("after sleep");	

	if (clock_gettime(CLOCK_REALTIME, &time_spec) < 0)
		printf("[ERROR]clock_gettime error %s \n", strerror(errno));
		
	print_time("before nanosleep");	
	time_spec.tv_sec = 3;	
	time_spec.tv_nsec = 0;
	if (nanosleep(&time_spec, &old_time_spec) < 0)
		printf("[ERROR]nanosleep error %s \n", strerror(errno));
	print_time("after nanosleep");
	
	if (clock_gettime(CLOCK_REALTIME, &time_spec) < 0)
		printf("[ERROR]clock_gettime error %s \n", strerror(errno));
		
	printf("before clock_nanosleep, sec:%ld, nsec:%ld \n", time_spec.tv_sec, time_spec.tv_nsec);		
	time_spec.tv_sec += 3;		
	
	if ( clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &time_spec, NULL) != 0)
		printf("[ERROR]clock_nanosleep error %s \n", strerror(errno));
			
	print_time("after clock_nanosleep");
	exit(0);
}


static void print_time(const  char* str)
{
	struct timespec time_now;
	memset(&time_now, 0x00, sizeof(time_now));
	
	if (NULL == str)
		return;
		
	printf("%s ", str);
	if (clock_gettime(CLOCK_REALTIME, &time_now) < 0)
		printf("[ERROR]clock_gettime error %s \n", strerror(errno));
		
	printf("sec:%ld, nsec:%ld \n", time_now.tv_sec, time_now.tv_nsec);		
}