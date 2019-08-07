#include "apue.h"
#include <signal.h>
#include <bits/signum.h>
#include <sys/wait.h>

#ifdef SOLARIS
#define PSCMD	"ps -a -o pid,ppid,s,tty,comm"
#else
#define PSCMD	"ps -o pid -o ppid -o state -o tty -o command"
#endif

int main(void)
{
	pid_t	pid;
	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
		err_sys("signal error");	
				
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0)		/* child */
	{
		if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
			err_sys("signal error");	
		exit(0);
	}

	/* parent */
	sleep(4);
	system(PSCMD);

	exit(0);
}
