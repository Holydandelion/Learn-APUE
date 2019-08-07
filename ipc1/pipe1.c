#include "apue.h"
#include <unistd.h>
#include <fcntl.h>


int
main(void)
{
	int		n;
	int		fd[2];
	pid_t	pid;
	int log_fd;
	char	line[MAXLINE];

	if (pipe(fd) < 0)
		err_sys("pipe error");
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid > 0) {		/* parent */
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		/*
		write(fd[1], "hello world\n", 12);
		*/
		printf("hello world \n");
		sleep(3);
	} else {					/* child */
		close(fd[1]);
		sleep(6);
		n = read(fd[0], line, MAXLINE);
		log_fd = open("./log", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
		write(log_fd, line, n);
		
	}
	exit(0);
}
