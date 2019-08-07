#include <signal.h> 
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

static void handle_sigusr(int signo, siginfo_t *siginfo, void *context);

int main(void)
{
	struct sigaction act;
	struct sigaction oact;
	
	memset(&act, 0x00, sizeof(act));
	memset(&oact, 0x00, sizeof(oact));
	
	act.sa_handler = NULL;
	sigemptyset(&act.sa_mask);
	act.sa_flags |= SA_SIGINFO;
	act.sa_sigaction =  handle_sigusr;
	
	if (sigaction(SIGUSR1, &act, &oact) < 0)
		printf("[ERROR]sigaction error, %s \n", strerror(errno));

	if (sigaction(SIGUSR2, &act, &oact) < 0)
		printf("[ERROR]sigaction error, %s \n", strerror(errno));
		
	for (; ;)
		pause();
			
	exit(0);
}

static void handle_sigusr(int signo, siginfo_t *siginfo, void *context)
{
	siginfo_t *p = siginfo;
	if (signo == SIGUSR1)
		printf("received SIGUSR1 ");
	else if (signo == SIGUSR2)
		printf("received SIGUSR2 ");
	else
		printf("[ERROR]signo error, %d \n", signo);
		
	printf("from pid:%d, user_id:%d \n", p->si_pid, p->si_uid);	
	fflush(stdout);
}
