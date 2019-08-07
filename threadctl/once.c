

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

pthread_once_t init_control = PTHREAD_ONCE_INIT;

void handle_sigusr(int signo)
{
	printf("thread:%lx caught sigusr1 \n", pthread_self());	
}

void dynamic_init(void)
{	
	printf("tid:%lx \n", pthread_self());	
}

void * thread_start(void *arg)
{
	int err = -1;
	int signo = 0;
	sigset_t thread_sigset;
	
	err = pthread_once(&init_control, dynamic_init);	
	if (err != 0)
	{
		printf("pthread_once failed. \n");
		exit(1);
	}	

	sigemptyset(&thread_sigset);
	sigaddset(&thread_sigset, SIGUSR1);	
	sigwait(&thread_sigset, &signo);
	printf("caught signal:%d \n", signo);
	
	return (void *)arg;
}

int main(void)
{
	pthread_t tid;
	int err = -1;
	int *ret_val = NULL;
	sigset_t thread_sigset;
	
	printf("main thread:%lx \n", pthread_self());
	sigemptyset(&thread_sigset);
	sigaddset(&thread_sigset, SIGUSR1);
	err = pthread_sigmask(SIG_BLOCK, &thread_sigset, NULL);
	if (err != 0)
	{
		printf("pthread_sigmask failed. \n");
		exit(1);
	}	
	
	err = pthread_create(&tid, NULL, thread_start, (void *)0);
	if (err != 0)
	{
		printf("pthread_create failed. \n");
		exit(1);
	}	
	
	sleep(1);
	err = pthread_once(&init_control, dynamic_init);	
	if (err != 0)
	{
		printf("pthread_once failed. \n");
		exit(1);
	}	
	
	pthread_join(tid, (void **)&ret_val);
	printf("exit from main thread.\n");
	exit(0);
}