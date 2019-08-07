#include "apue.h"
#include <pthread.h>
#include <stdlib.h>

pthread_t ntid;

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

int main(void)
{
	int		err;
	thread_exit_info	*thread_ret = NULL;
	char *argv[] = {"arg0", "arg1", "arg2", NULL};	
	
	err = pthread_create(&ntid, NULL, thr_fn, argv);
	if (err != 0)
		err_exit(err, "can't create thread");
		
	printids("main thread:");
	pthread_join(ntid, (void **)&thread_ret); 
	if (thread_ret != NULL && thread_ret != PTHREAD_CANCELED)
	{
		printf("join thread:%lx \n", ntid);		
		printf("return info code:%d, str:%s \n", thread_ret->code, thread_ret->str);
		free((void *)thread_ret);
	}
	
	exit(0);
}
