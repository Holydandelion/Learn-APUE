#include "apue.h"
#include <sys/wait.h>

int
main(void)
{
	int		status;

	if ((status = system("date")) < 0)
		err_sys("system() error");

	pr_exit(status);
	
	if (WIFEXITED(status) && (WEXITSTATUS(status) == 0))
		printf("execute cmd succeed. \n");
	else
		printf("execute cmd failed. \n");		

	if ((status = system("nosuchcommand")) < 0)
		err_sys("system() error");

	pr_exit(status);
	
	if (WIFEXITED(status) && (WEXITSTATUS(status) == 0))
		printf("execute cmd succeed. \n");
	else
		printf("execute cmd failed. \n");		

	if ((status = system("who; exit 44")) < 0)
		err_sys("system() error");

	pr_exit(status);
	if (WIFEXITED(status) && (WEXITSTATUS(status) == 0))
		printf("execute cmd succeed. \n");
	else
		printf("execute cmd failed. \n");		

	exit(0);
}
