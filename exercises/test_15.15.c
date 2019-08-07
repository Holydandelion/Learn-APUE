#include "apue.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define	NLOOPS		1000
#define	SIZE		sizeof(long)	/* size of shared memory area */
#define SHM_SIZE 	4096

static int
update(long *ptr)
{
	return((*ptr)++);	/* return value before increment */
}

int
main(void)
{
	int		i, counter;
	pid_t	pid;
	void	*area = NULL;
	int shm_id = -1;
	int sem_id = -1;
	unsigned short init_value[2] = {0, 0};
	struct sembuf sembuf_array[1];
	
	if ((shm_id = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT|0664)) == -1)
		err_sys("mmap error");

	if ((sem_id = semget(IPC_PRIVATE, 2, IPC_CREAT|0664)) == -1)
		err_sys("mmap error");

	if (semctl(sem_id, 2, SETALL, init_value) < 0)
		err_sys("shmctl error");	

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
				
			sembuf_array[0].sem_num = 1;
			sembuf_array[0].sem_op = 1;
			sembuf_array[0].sem_flg = SEM_UNDO;			
			if (semop(sem_id, sembuf_array, 1) == -1)
				err_sys("semop error");	
				
			sembuf_array[0].sem_num = 0;
			sembuf_array[0].sem_op = -1;
			sembuf_array[0].sem_flg = SEM_UNDO;			
			if (semop(sem_id, sembuf_array, 1) == -1)
				err_sys("semop error");									
							
		}
		if (shmdt(area) == -1)
			err_sys("shmdt error");	
					
		if (shmctl(shm_id, IPC_RMID, NULL) < 0)
			err_sys("shmctl error");	
			
		if (semctl(sem_id, 2, IPC_RMID, 0) < 0)
			err_sys("shmctl error");					
	} else 
	{
		/* child */
		if ((area = shmat(shm_id, 0, 0)) == (void *)-1)
			err_sys("shmat error");	
			
		for (i = 1; i < NLOOPS + 1; i += 2) 
		{
			sembuf_array[0].sem_num = 1;
			sembuf_array[0].sem_op = -1;
			sembuf_array[0].sem_flg = SEM_UNDO;
			if (semop(sem_id, sembuf_array, 1) == -1)
				err_sys("child semop error");

			if ((counter = update((long *)area)) != i)
				err_quit("child: expected %d, got %d", i, counter);

			printf("child update, counter:%d\n", counter);

			sembuf_array[0].sem_num = 0;
			sembuf_array[0].sem_op = 1;
			sembuf_array[0].sem_flg = SEM_UNDO;
			if (semop(sem_id, sembuf_array, 1) == -1)
				err_sys("child semop error");			
		}		
		if (shmdt(area) == -1)
			err_sys("child shmdt error");					
	}

	exit(0);
}
