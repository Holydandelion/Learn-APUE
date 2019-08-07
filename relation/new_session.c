
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	pid_t pid = -1;
	pid_t sid = -1;
	char c = 0;
	
	if ((pid = fork()) == -1)
		printf("[ERROR]fork error, %s \n", strerror(errno));
	else if (pid == 0)
	{
		if ((sid = setsid()) == -1)
			printf("[ERROR]setsid error, %s \n", strerror(errno));
		
		printf("pid:%d, ppid:%d, pgid:%d, sid:%d \n", getpid(), getppid(), getpgid(0), getsid(0));
		
		if (read(STDIN_FILENO, &c, 1) != 1)
			printf("[ERROR]read error, %s \n", strerror(errno));
			
		printf("read c:%c \n", c);	
		
		if (tcsetpgrp(STDOUT_FILENO, sid) == -1)
			printf("[ERROR]tcsetpgrp error, %s \n", strerror(errno));
		else
			printf("set tcpgid succeed. tcpgid:%d \n", tcgetpgrp(STDOUT_FILENO));	
			
		fflush(stdout);		
		pause();			
	}	
	
	sleep(5);	
	exit(0);
}
