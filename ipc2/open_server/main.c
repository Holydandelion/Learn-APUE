
#include "open_server.h"
#include "daemonize.h"
#include <stdlib.h>
#include <unistd.h>

int		debug;
int		log_to_stderr;
int 	oflag;
int 	client_size;
char	errmsg[MAXLINE] = {0};
char	*pathname = NULL;
Client	*client_array = NULL;

void exit_handler(void)
{
	unlink(SERVER_PATH);
}

int main(int argc, char *argv[])
{
	int		c;

	opterr = 0;		/* don't want getopt() writing to stderr */
	while ((c = getopt(argc, argv, "d")) != -1) 
	{
		switch (c) 
		{
			case 'd':		/* debug */
				debug = 1;
				log_to_stderr = 1;
				break;
	
			case '?':
				printf("unrecognized option: -%c", optopt);
				exit(1);
		}
	}

	if (debug == 0)
		daemonize("opend");

	if (atexit(exit_handler) < 0)
	{
		printf("atexit error, %s", strerror(errno));
		exit(1);	
	}
	
	loop();		/* never returns */
	exit(0);
}
