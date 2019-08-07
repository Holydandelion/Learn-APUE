

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>

#define SHM_PATH "./shm_key"
#define SHM_MODE 0664
#define SHM_SIZE 1048576

void handle_sigusr1(int signo)
{
	printf("caught SIGUSR1\n");
}

int main(int argc, char *argv[])
{
	key_t key = -1;
	int shm_id = -1;
	int shm_flag = 0;
	size_t shm_size = 0;
	struct shmid_ds shm_obj;
	void *shm_addr = NULL;
	char buf[] = "This is from server.";
	
	if (argc != 2)
	{
		printf("must be 2 arguments.\n");
		exit(1);
	}	
	signal(SIGUSR1, handle_sigusr1);
	if ((key = ftok(SHM_PATH, 0)) < 0)
	{
		printf("ftok error, %s\n", strerror(errno));
		exit(1);
	}
	printf("key:%x\n", key);
	
	if (memcmp("server", argv[1], 6) == 0)
		shm_size = SHM_SIZE;
	else
	{
		shm_size = 0;
		shm_flag = SHM_RDONLY;	
	}
			
	if ((shm_id = shmget(key, shm_size, SHM_MODE|IPC_CREAT)) < 0)
	{
		printf("shmget error, %s\n", strerror(errno));
		exit(1);
	}	
	
	if ((shm_addr = shmat(shm_id, 0, shm_flag)) < 0)
	{
		printf("shm_addr error, %s\n", strerror(errno));
		exit(1);
	}	
	
	if (shmctl(shm_id, IPC_STAT, &shm_obj) < 0)
	{
		printf("shmctl error, %s\n", strerror(errno));
		exit(1);
	}	
	
	printf("uid:%d, gid:%d\n", shm_obj.shm_perm.uid, shm_obj.shm_perm.gid);
	printf("mode:%o\n", shm_obj.shm_perm.mode);		
	printf("size:%zd\n", shm_obj.shm_segsz);
	printf("last operate pid:%d\n", shm_obj.shm_lpid);
	printf("attach processes:%d\n", (int)shm_obj.shm_nattch);		


 	if (memcmp("server", argv[1], 6) == 0)
 	{
 		memcpy(shm_addr, buf, sizeof(buf));
 	}
 	else
 	{
 		memset(buf, 0x00, sizeof(buf));
 		memcpy(buf, shm_addr, sizeof(buf));
 		printf("from server:\n %s \n", buf);
 	}

	pause();	
	if (shmdt(shm_addr) < 0)
	{
		printf("shmdt error, %s\n", strerror(errno));
		exit(1);
	}	

	if (memcmp("server", argv[1], 6) == 0)
	{	
		if (shmctl(shm_id, IPC_RMID, &shm_obj) < 0)
		{
			printf("shmctl error, %s\n", strerror(errno));
			exit(1);
		}	
		printf("Remove shared memory identifier.\n");
	}	
	exit(0);
}