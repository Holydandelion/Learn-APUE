

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include "slock.h"

static struct slock *slock_ptr = NULL;

void *thread_fcn(void *arg)
{
	int err = -1;
	
	/*unlock PTHREAD_MUTEX_ERRORCHECK mutex will failed.*/	
	err = s_unlock(slock_ptr);
	if (err != 0)
		printf("unlock failed. %s\n", strerror(errno));
	else
		printf("unlock succeed by other thread. \n");
	
	err = s_unlock(slock_ptr);
	if (err != 0)
		printf("unlock failed. %s\n", strerror(errno));
	else
		printf("unlock succeed by other thread. \n");		

	return ((void *)arg);
}

int main(void)
{
	int err = 0;
	pthread_t tid;
	void *thread_ret = NULL;
	
	slock_ptr = s_alloc();
	if (slock_ptr == NULL)
	{
		printf("s_alloc failed\n");
		exit(1);
	}

	err = s_lock(slock_ptr);
	if (err == 0)
		printf("lock by self succeed. \n");
	else
		printf("s_lock failed.%s \n", strerror(errno));			

	err = pthread_create(&tid, NULL, thread_fcn, (void *)NULL);
	if (err != 0)
	{
		printf("[%d]pthread_create failed\n", err);
		exit(1);
	}

	err = pthread_join(tid, &thread_ret);
	exit(0);
}

