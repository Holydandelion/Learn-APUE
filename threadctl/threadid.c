#include "apue.h"
#include <pthread.h>
#include <stdlib.h>

pthread_t tid;

typedef struct thread_exit_info
{
	int code;
	char str[128];
}thread_exit_info;

void
printids(const char *s)
{
	pid_t		pid;
	pthread_t	tid;

	pid = getpid();
	tid = pthread_self();
	printf("%s pid %lu tid %lu (0x%lx)\n", s, (unsigned long)pid,
	  (unsigned long)tid, (unsigned long)tid);
}

void *
thr_fn(void *arg)
{
	unsigned int i = 0;
	char **argv = arg;
	thread_exit_info *exit_info_ptr = NULL; 	
	
	printids("new thread: ");
	sleep(3);	
	for (;argv[i] != NULL;i++)
		printf("arg[%d]:%s ", i, argv[i]);	
	printf("\n");

	exit_info_ptr = (thread_exit_info *)malloc(sizeof(thread_exit_info));
	if  (exit_info_ptr == NULL)
	{
		printf("malloc error \n");		
		return((void *) NULL);
	}
	exit_info_ptr->code = 1;
	strcpy(exit_info_ptr->str, "normal exit");
	return ((void *)exit_info_ptr);
}

char *argv[] = {"arg0", "arg1", "arg2", NULL};	
char *str = "detached";

int main(void)
{
	int		err;
	thread_exit_info	*thread_ret = NULL;


	pthread_attr_t thread_attr;
	
	err = pthread_create(&tid, NULL, thr_fn, argv);
	if (err != 0)
		err_exit(err, "can't create thread");
		
	printids("main thread:");
	pthread_join(tid, (void **)&thread_ret);
	
	if (thread_ret != NULL && thread_ret != PTHREAD_CANCELED)
	{
		printf("join thread:%lx \n", tid);		
		printf("return info code:%d, str:%s \n", thread_ret->code, thread_ret->str);
		free((void *)thread_ret);
	}
	
	err = pthread_attr_init(&thread_attr);
	if (err != 0)
		err_exit(err, "pthread_attr_init failed.");
		
	err = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if (err != 0)
		err_exit(err, "pthread_attr_setdetachstate failed.");	
	
	argv[0] = str;
	err = pthread_create(&tid, &thread_attr, thr_fn, argv);
	if (err != 0)
		err_exit(err, "can't create thread");			
	
	pthread_attr_destroy(&thread_attr);
	
	/*only main thread sleep, the new thread is running.*/
	sleep(5);  
	return 0;
}
