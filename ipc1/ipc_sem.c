

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define KEY_PATH "./key"
#define IPC_MODE 0664
#define SEM_VAL 3 
#define NSEMS 2

void handle_sigint(int signo)
{
	printf("caught SIGINT \n");
}

int main(int argc, char *argv[])
{
	struct semid_ds sem;
	struct sembuf semops[NSEMS];
	key_t sem_key;
	int sem_id = -1;
	int sem_num = 1;
	int nsems = 0;
	int val = -1;
	
	if (argc !=  2)
	{
		printf("arguments error \n");
		exit(1);
	}
	
	signal(SIGINT, handle_sigint);
	
	if ((sem_key = ftok(KEY_PATH, 0)) < 0)
	{
		printf("ftok error, %s\n", strerror(errno));
		exit(1);
	}		
	printf("get key, sem_key:%x \n", sem_key);
	
	if (memcmp("server", argv[1], 6) == 0)
		nsems = NSEMS;
	else
		nsems = 0;
		
	if ((sem_id = semget(sem_key, nsems, IPC_MODE|IPC_CREAT)) < 0)
	{		
		printf("semget error, %s\n", strerror(errno));
		exit(1);		
	}	
	
	if (memcmp("server", argv[1], 6) == 0)
	{	
		if (semctl(sem_id, sem_num, SETVAL, SEM_VAL) < 0)
		{
			printf("semctl error, %s\n", strerror(errno));
			exit(1);		
		}
	}	
	
	memset(&semops, 0x00, sizeof(semops));
	semops[0].sem_num = sem_num;
	semops[0].sem_op = -1;
	semops[0].sem_flg = SEM_UNDO;		
	
	if (semop(sem_id, semops, 1) < 0)
	{
		printf("semop error, %s\n", strerror(errno));
		exit(1);		
	}
	
	if (semctl(sem_id, sem_num, IPC_STAT, &sem) < 0)
	{
		printf("semctl error, %s\n", strerror(errno));
		exit(1);		
	}	
	
	printf("uid:%d gid:%d \n", sem.sem_perm.uid, sem.sem_perm.gid);
	printf("mode:%o \n", sem.sem_perm.mode);
	printf("sem.sem_nsems:%d \n", sem.sem_nsems);
	printf("sem.sem_otime:%d sem.sem_ctime:%s \n", sem.sem_otime, ctime(&sem.sem_ctime));	

	if ((val = semctl(sem_id, sem_num, GETVAL, 0)) < 0)
	{
		printf("semctl error, %s\n", strerror(errno));
		exit(1);		
	}	
	printf("get sem val:%d \n", val);

	pause();	
	if (memcmp("server", argv[1], 6) == 0)
	{
		if ((val = semctl(sem_id, sem_num, IPC_RMID, 0)) < 0)
		{
			printf("semctl error, %s\n", strerror(errno));
			exit(1);		
		}	
		printf("semaphore removed.\n");
	}
	else
	{
		memset(&semops, 0x00, sizeof(semops));
		semops[0].sem_num = sem_num;
		semops[0].sem_op = 1;
		semops[0].sem_flg = SEM_UNDO;		
		
		if (semop(sem_id, semops, 1) < 0)
		{
			printf("semop error, %s pid:%d\n", strerror(errno), getpid());
			exit(1);		
		}		
	}	

	exit(0);
}