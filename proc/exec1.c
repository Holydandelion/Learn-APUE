#include "apue.h"
#include <sys/wait.h>

char	*env_init[] = { "USER=unknown", "PATH=/tmp", NULL };
char 	*argv_init[] = { "arg0", "arg1", "arg2", "the last arg", NULL};

int
main(void)
{
	pid_t	pid;

	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {	/* specify pathname, specify environment */
		printf("execle ./echoall \n");
		if (execle("./echoall", "echoall", "myarg1",
				"MY ARG2", (char *)0, env_init) < 0)
			err_sys("execle error");
	}

	if (waitpid(pid, NULL, 0) < 0)
		err_sys("wait error");

	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {	/* specify filename, inherit environment */
		printf("execlp echoall \n");		
		if (execlp("echoall", "echoall", "only 1 arg", (char *)0) < 0)
			err_sys("execlp error");
	}

	if (waitpid(pid, NULL, 0) < 0)
		err_sys("wait error");

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0)
	{
		printf("execve ./echoall \n");		
		if (execve("./echoall", argv_init, env_init) == -1)
			err_sys("execve error");		
		exit(1); /*if normal, can not reach here.*/
	}		

	if (waitpid(pid, NULL, 0) < 0)  /*if do not wait child proc, shell $ prompt will print ahead.*/
		err_sys("wait error");
	
	exit(0);
}
