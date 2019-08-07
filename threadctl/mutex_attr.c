#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "apue.h"

pthread_mutexattr_t mutex_attr;
pthread_mutex_t mutex;

void *thread_fn1(void *arg)
{
	int err = -1;
	err = pthread_mutex_trylock(&mutex);
	/* no blocking, return error*/
	if (err != 0)
		printf("[ERROR]lock failed. \n");
	return ((void *)arg);
}


int main(void)
{
	int		err = -1;
	int 	type = PTHREAD_MUTEX_NORMAL;
	void 	*tret = NULL;
	pthread_t	tid;
		
	printf("main thread:0x%lx\n ", pthread_self());
	
	err = pthread_mutexattr_init(&mutex_attr);
	if (err != 0)
		err_exit(err, "pthread_attr_init failed.");	

	err = pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
	if (err != 0)
		err_exit(err, "pthread_mutexattr_settype failed.");	

	err =pthread_mutex_init(&mutex, &mutex_attr);
	if (err != 0)
		err_exit(err, "pthread_mutex_init failed.");		

	pthread_mutexattr_gettype(&mutex_attr, &type);
	if (type == PTHREAD_MUTEX_RECURSIVE)	
		printf("already set mutex type:PTHREAD_MUTEX_RECURSIVE \n");

	pthread_mutex_lock(&mutex);	
	pthread_mutexattr_destroy(&mutex_attr);

	err = pthread_mutex_lock(&mutex);
	if (err == 0)
		printf("[INFO]lock again by self succeed. \n");
	else
		printf("[INFO]atempt to lock again by self. \n");		
				
	err = pthread_create(&tid, NULL, thread_fn1, (void *)NULL);
	if (err != 0)
		err_exit(err, "can't create thread 1");	
	
	err = pthread_join(tid, &tret);
	if (err != 0)
		err_exit(err, "can't join thread 1");			

	exit(0);
}
