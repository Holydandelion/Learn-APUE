
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define MAX_TIME 64

int main(void)
{
	time_t t_now = 0;
	time_t t_max = 0;
	struct tm *tm_ptr = NULL;
	char buf[MAX_TIME] = {0};
	
	if (time(&t_now) == -1)
		printf("[ERROR]time error,%s\n", strerror(errno));
	
	t_max = (1ULL << (sizeof(time_t) * 8 - 1)) - 1;
	printf("[DEBUG]t_now:%ld, t_max:%ld \n", t_now, t_max);
	
	/*gmtime cannot support time_t lager then int*/
	t_max = (1U << (sizeof(int) * 8 - 1)) - 1;
	if ((tm_ptr = gmtime(&t_max)) == NULL)
		printf("[ERROR]gmtime error,%s\n", strerror(errno));

	if (strftime(buf, sizeof(buf), "%a %b %d, %Y %H:%M:%S", tm_ptr) == 0)
		printf("[ERROR]strftime error,%s\n", strerror(errno));
	
	printf("[DEBUG]buf is %s \n", buf);
	
	exit(0);
}