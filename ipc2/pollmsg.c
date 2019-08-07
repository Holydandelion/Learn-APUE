#include "apue.h"
#include <poll.h>
#include <pthread.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/ipc.h>

#define NQ		3		/* number of queues */
#define MAXMSZ	512		/* maximum message size */
#define KEY_PATH		"/home/chenruogeng/Learning/APUE/apue.3e/ipc2/queue.key"	/* path for create key*/

struct threadinfo 
{
	int qid;
	int fd;
};

static struct threadinfo	ti[NQ];

struct mymesg 
{
	long mtype;
	char mtext[MAXMSZ];
};

void * thread_start(void *arg)
{
	int					n;
	struct mymesg		m;
	struct threadinfo	*tip = arg;

	for(;;) 
	{
		memset(&m, 0, sizeof(m));
		if ((n = msgrcv(tip->qid, &m, MAXMSZ, 0, MSG_NOERROR)) < 0)
			err_sys("msgrcv error");
		if (write(tip->fd, m.mtext, n) < 0)
			err_sys("write error");
	}
}

int main()
{
	int					i, n, err;
	int					fd[2];
	int					qid[NQ];
	struct pollfd		pfd[NQ];
	pthread_t			tid[NQ];
	char				buf[MAXMSZ];
	key_t 				key = -1;
	
	for (i = 0; i < NQ; i++) 
	{
		if ((key = ftok(KEY_PATH, i)) < 0)
			err_sys("ftok error");
						
		if ((qid[i] = msgget(key, IPC_CREAT|0666)) < 0)
			err_sys("msgget error");

		printf("queue ID %d is %d\n", i, qid[i]);

		if (socketpair(AF_UNIX, SOCK_DGRAM, 0, fd) < 0)
			err_sys("socketpair error");
			
		pfd[i].fd = fd[0];
		pfd[i].events = POLLIN;
		
		ti[i].qid = qid[i];
		ti[i].fd = fd[1];
		if ((err = pthread_create(&tid[i], NULL, thread_start, &ti[i])) != 0)
			err_exit(err, "pthread_create error");
	}

	for (;;) 
	{
		if (poll(pfd, NQ, -1) < 0)
			err_sys("poll error");
			
		for (i = 0; i < NQ; i++) 
		{
			if (pfd[i].revents & POLLIN) 
			{
				if ((n = read(pfd[i].fd, buf, sizeof(buf))) < 0)
					err_sys("read error");
				buf[n] = 0;
				printf("queue id %d, message %s\n", qid[i], buf);
			}
		}
	}

	exit(0);
}
