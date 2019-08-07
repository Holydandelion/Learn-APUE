#include "apue.h"
#include <sys/wait.h>
#include <unistd.h>

int
main(void)
{
	pid_t	pid;
	int		status;

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0)				/* child */
	{
		printf("[DEBUG]pid:%d, ppid:%d \n", getpid(), getppid());
		exit(7);
	}

	if (wait(&status) != pid)		/* wait for child */
		err_sys("wait error");
	pr_exit(status);				/* and print its status */

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0)				/* child */
	{
		printf("[DEBUG]pid:%d, ppid:%d \n", getpid(), getppid());	
		abort();					/* generates SIGABRT */
	}
	
	if (wait(&status) != pid)		/* wait for child */
		err_sys("wait error");
	pr_exit(status);				/* and print its status */

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0)				/* child */
	{
		printf("[DEBUG]pid:%d, ppid:%d \n", getpid(), getppid());	
		status /= 0;				/* divide by 0 generates SIGFPE */
	}
	
	if (wait(&status) != pid)		/* wait for child */
		err_sys("wait error");
	pr_exit(status);				/* and print its status */

	exit(0);
}
