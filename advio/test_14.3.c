

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "myselect.h"
#include <sys/select.h>
#include <stdlib.h>



int main(void)
{
	struct timeval time_val;
	fd_set file_desc_set;
	time_val.tv_sec = 1;
	time_val.tv_usec = 0;
	
	FD_ZERO(&file_desc_set);	
	FD_SET(2047, &file_desc_set);
		
	if (select(2048, &file_desc_set, NULL, NULL, &time_val) < 0)
	{
		printf("select failed, %s\n", strerror(errno));
		exit(1);
	}
	printf("FD_SETSIZE=%d \n", FD_SETSIZE);
	exit(0);

}
