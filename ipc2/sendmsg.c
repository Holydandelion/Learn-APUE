#include "apue.h"
#include <sys/msg.h>
#include <sys/ipc.h>

#define MAXMSZ 512
#define KEY_PATH		"/home/chenruogeng/Learning/APUE/apue.3e/ipc2/queue.key"	/* path for create key*/

struct mymesg {
	long mtype;
	char mtext[MAXMSZ];
};

int
main(int argc, char *argv[])
{
	long 	qid;
	size_t 	nbytes;
	struct 	mymesg m;
	key_t		key = -1;	
	long 	id;

	if (argc != 3) 
	{
		fprintf(stderr, "usage: sendmsg id message\n");
		exit(1);
	}
	id = strtol(argv[1], NULL, 0);
	
	if ((key = ftok(KEY_PATH, id)) < 0)
		err_sys("ftok error");	
		
	if ((qid = msgget(key, 0)) < 0)
		err_sys("can't open queue key %s", argv[1]);
	memset(&m, 0, sizeof(m));
	strncpy(m.mtext, argv[2], MAXMSZ-1);
	nbytes = strlen(m.mtext);
	m.mtype = 1;
	if (msgsnd(qid, &m, nbytes, 0) < 0)
		err_sys("can't send message");
	exit(0);
}
