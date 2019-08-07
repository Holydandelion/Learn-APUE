

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex;

void *thread_fcn(void *arg)
{
	int err = -1;
	
	/*unlock PTHREAD_MUTEX_ERRORCHECK mutex will failed.*/	
	err = pthread_mutex_unlock(&mutex);
	if (err != 0)
		printf("[%d]unlock failed by other thread.\n", err);
	else
		printf("unlock succeed by other thread. \n");

	return ((void *)arg);
}

int main(void)
{
	int err = 0;
	pthread_mutexattr_t mutex_attr;
	pthread_t tid;
	void *thread_ret = NULL;
	
	err = pthread_mutexattr_init(&mutex_attr);
	if (err != 0)
	{
		printf("[%d]pthread_mutexattr_init failed\n", err);
		exit(1);
	}

	err = pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
	if (err != 0)
	{
		printf("[%d]pthread_mutexattr_settype failed\n", err);
		exit(1);
	}

	err = pthread_mutex_init(&mutex, &mutex_attr);
	if (err != 0)
	{
		printf("[%d]pthread_mutex_init failed\n", err);
		exit(1);
	}

	pthread_mutexattr_destroy(&mutex_attr);

	err = pthread_mutex_lock(&mutex);
	if (err == 0)
		printf("lock by self succeed. \n");
	else
		printf("try to lock again by self. \n");	
	
	err = pthread_mutex_unlock(&mutex);
	if (err != 0)
		printf("[%d]unlock failed. \n", err);
	else
		printf("unlock succeed. \n");

	/* unlock again*/
	err = pthread_mutex_unlock(&mutex);
	if (err != 0)
		printf("[%d]unlock again failed. \n", err);
	else
		printf("unlock again succeed. \n");	
		
	err = pthread_mutex_lock(&mutex);
	if (err == 0)
		printf("lock by self succeed. \n");
	else
		printf("try to lock again by self. \n");				

	err = pthread_create(&tid, NULL, thread_fcn, (void *)NULL);
	if (err != 0)
	{
		printf("[%d]pthread_create failed\n", err);
		exit(1);
	}

	err = pthread_join(tid, &thread_ret);
	
	exit(0);
}

