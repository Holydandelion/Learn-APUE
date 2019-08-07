#include "apue.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <semaphore.h>

#define	NLOOPS		1000
#define	SIZE		sizeof(long)	/* size of shared memory area */
#define SHM_SIZE 	4096
#define PSHARED 	1
#define SEM_MODE 	S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH

static int update(long *ptr)
{
	return((*ptr)++);	/* return value before increment */
}

int main(void)
{
	int		i, counter;
	pid_t	pid;
	void	*area = NULL;
	int shm_id = -1;
	sem_t *sem_parent_ptr;
	sem_t *sem_child_ptr;	
	
	if ((shm_id = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT|0664)) == -1)
		err_sys("mmap error");
	
	if ((sem_parent_ptr = sem_open("/test_15.18.1", O_CREAT, SEM_MODE, 0)) == SEM_FAILED)
		err_sys("sem_open error");

	if ((sem_child_ptr = sem_open("/test_15.18.2", O_CREAT, SEM_MODE, 0)) == SEM_FAILED)
		err_sys("sem_open error");
		
	if ((pid = fork()) < 0) 
	{
		err_sys("fork error");
	} else if (pid > 0) 
	{			
		/* parent */
		if ((area = shmat(shm_id, 0, 0)) == (void *)-1)
			err_sys("shmat error");				
					
		for (i = 0; i < NLOOPS; i += 2) 
		{
			if ((counter = update((long *)area)) != i)
				err_quit("parent: expected %d, got %d", i, counter);
			printf("parent update, counter:%d\n", counter);	
					
			if (sem_post(sem_child_ptr) == -1)
				err_sys("sem_post error");									

			if (sem_wait(sem_parent_ptr) == -1)
				err_sys("sem_wait error");								
		}
		if (shmdt(area) == -1)
			err_sys("shmdt error");	
					
		if (shmctl(shm_id, IPC_RMID, NULL) < 0)
			err_sys("shmctl error");	
			
		if (sem_close(sem_child_ptr) == -1)
			err_sys("sem_destroy error");		

		if (sem_close(sem_parent_ptr) == -1)
			err_sys("sem_destroy error");		
	} else 
	{
		/* child */
		if ((area = shmat(shm_id, 0, 0)) == (void *)-1)
			err_sys("shmat error");	
			
		for (i = 1; i < NLOOPS + 1; i += 2) 
		{
			if (sem_wait(sem_child_ptr) == -1)
				err_sys("sem_post error");	

			if ((counter = update((long *)area)) != i)
				err_quit("child: expected %d, got %d", i, counter);

			printf("child update, counter:%d\n", counter);

			if (sem_post(sem_parent_ptr) == -1)
				err_sys("sem_post error");				
		}		
		if (shmdt(area) == -1)
			err_sys("child shmdt error");					
	}

	exit(0);
}
