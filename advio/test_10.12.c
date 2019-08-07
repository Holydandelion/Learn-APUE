

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

static void sig_alrm(int signo);

static char buffer[500000] = {1};

int main(void)
{
	FILE *fp;
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
	{
		printf("signal error, %s \n", strerror(errno));
		exit(1);
	}
	
	fp = fopen("./templog", "w+");	
	/* alarm(1); */
	
	fwrite(buffer,  sizeof(buffer), 1, fp);
	printf("fwrite returned, %s \n", strerror(errno));
	exit(0);
}

static void sig_alrm(int signo)
{
	printf("caught signal alarm \n");
	return;
}