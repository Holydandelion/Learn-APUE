
#include "my_daemonize.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/resource.h>
#include <limits.h>
#include <fcntl.h>
#include <syslog.h>

#define LOGMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

void my_daemonize(const char *cmd)
{
	pid_t pid = -1;
	struct sigaction act;
	struct rlimit file_rlimit;
	int i = 0;
	int fd0 = -1;
	int fd1 = -1;
	int fd2 = -1;
	char log_path[PATH_MAX] = {0};

	
	if (cmd == NULL)
		return;
			
	umask(0);
	
	if ((pid = fork()) < 0)
	{
		printf("fork error, %s\n", strerror(errno));
		exit(1);
	}
	else if (pid > 0)
		exit(0);
		
	setsid();
	
	act.sa_handler = SIG_IGN;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(SIGHUP, &act, NULL) < 0)
	{
		printf("sigaction error, %s\n", strerror(errno));
		exit(1);		
	}
	
	if ((pid = fork()) < 0)
	{
		printf("fork error, %s\n", strerror(errno));
		exit(1);
	}
	else if (pid > 0)
		exit(0);	
	
	if (chdir("/") < 0)
	{
		printf("chdir error, %s\n", strerror(errno));
		exit(1);
	}

	if (getrlimit(RLIMIT_NOFILE, &file_rlimit) < 0)
	{
		printf("gerrlimit error, %s\n", strerror(errno));
		exit(1);		
	}
	
	if (file_rlimit.rlim_max == RLIM_INFINITY)
		file_rlimit.rlim_max = 1024;
				
	for (i=0;i<file_rlimit.rlim_max;i++)
		close(i);
		
	snprintf(log_path, sizeof(log_path), "/log/%s.%d.log", cmd, getpid());	
	fd0 = open("/dev/null", O_RDWR|O_CREAT);	
	
	if ((fd1 = open(log_path, O_CREAT|O_RDWR, LOGMODE)) < 0)
	{
		syslog(LOG_ERR, "open fd1 error, %s\n", strerror(errno));
		exit(1);		
	}
		
	fd2 = dup(fd1);
	
	if (fd0 != 0 || fd1 != 1 || fd2 != 2)
	{
		syslog(LOG_ERR, "fd open error, fd0:%d, fd1:%d, fd2:%d\n", fd0, fd1, fd2);
		exit(1);		
	}	
	
	setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
	setvbuf(stderr, NULL, _IOLBF, BUFSIZ);
	
	printf("%s processing \n", cmd);
}
