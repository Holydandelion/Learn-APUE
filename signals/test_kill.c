
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

static int str2sig(const char *str, int *signo);

int main(int argc, char *argv[])
{
	int signo = 1;
	pid_t pid = -1;
	if (argc != 3)
	{
		printf("[ERROR]arguments error. \n");
		exit(1);
	}	
	
	for (;sys_siglist[signo] != NULL; signo++)
		printf("[DEBUG]  signo:%d, signal:%s \n", signo, sys_siglist[signo]);
	/* 
	if ((pid = strtol(argv[2], NULL, 10)) == 0)
		printf("[ERROR] argument pid error, %s\n", strerror(errno));		

	if (str2sig(argv[1], &signo) < 0)
		printf("[ERROR] str2sig error, %s\n", strerror(errno));
	
	if (kill(pid, signo) < 0)
		printf("[ERROR] kill error, %s\n", strerror(errno));

	printf("[DEBUG]pid:%d, signo:%d, signal:%s \n", pid, signo, sys_siglist[signo]);		
	*/
		
	exit(0);
}


static int str2sig(const char *str, int *signo)
{	
	if (NULL == str)
		return -1;
		
	if (strlen(str) != 7)
		return -1;
		
	if (strcmp(str, "SIGTSTP") == 0)
	{
		*signo = SIGTSTP;
		return 0; 	
	}
	
	if (strcmp(str, "SIGCONT") == 0)
	{
		*signo = SIGCONT;
		return 0; 	
	}

	if (strcmp(str, "SIGKILL") == 0)
	{
		*signo = SIGKILL;
		return 0; 	
	}
	
	/* todo */
	
	return -1;
}
