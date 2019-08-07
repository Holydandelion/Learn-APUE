
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 1024

typedef struct mymsg
{
	long m_type;
	char m_content[MSG_SIZE];
}mymsg;

int main(void)
{
	key_t key;
	int i = 0;
	int msg_id = -1;
	mymsg message;
	
	for (i=0;i<5;i++)
	{	
		key = ftok("./msg.key", i);
		printf("key:%x\n", key);	
		if ((msg_id = msgget(key, IPC_CREAT|0664)) == -1)
		{
			printf("msgget error, %s\n", strerror(errno));
			exit(1);
		}
		printf("msg_id:%d\n", msg_id);

		if (msgctl(msg_id, IPC_RMID, 0) < 0)
		{
			printf("msgget error, %s\n", strerror(errno));
			exit(1);
		}		
	}
	
	memset(&message, 0x00, sizeof(mymsg));
	message.m_type = 1;
	for (i=0;i<5;i++)
	{		
		if ((msg_id = msgget(IPC_PRIVATE, IPC_CREAT|0664)) == -1)
		{
			printf("msgget error, %s\n", strerror(errno));
			exit(1);
		}
		
		if (msgsnd(msg_id, &message, MSG_SIZE, 0) < 0)
		{
			printf("msgsnd error, %s\n", strerror(errno));
			exit(1);
		}	
		else
			printf("send message succeed.\n");	
	}	
	exit(0);

}
