#include "apue.h"
#include <fcntl.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

#define LOCKFILE "/var/run/test_daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define LOGMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

sigset_t	mask;

int lockfile(int fd)
{
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	return(fcntl(fd, F_SETLK, &fl));
}

pid_t lock_test(int fd, int type, off_t start, int whence, off_t len)
{
	struct flock file_lock;
	file_lock.l_type = type;
	file_lock.l_start = start;
	file_lock.l_whence = whence;
	file_lock.l_len = len;
	
	if (fcntl(fd, F_GETLK, &file_lock) < 0)
		printf("fcntl failed:%s \n", strerror(errno));
		
	if (file_lock.l_type == F_UNLCK)
		return 0;
	
	return (file_lock.l_pid);
}

void daemonize(const char *cmd)
{
	int					i;
	int 				fd0 = -1;
	int 				fd1 = -1;
	int 				fd2 = -1;	
	pid_t				pid;
	struct rlimit		rl;
	struct sigaction	sa;
	char logfile[64] = {0};
	/*
	 * Clear file creation mask.
	 */
	umask(0);

	/*
	 * Get maximum number of file descriptors.
	 */
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		err_quit("%s: can't get file limit", cmd);

	/*
	 * Become a session leader to lose controlling TTY.
	 */
	if ((pid = fork()) < 0)
		err_quit("%s: can't fork", cmd);
	else if (pid != 0) /* parent */
		exit(0);
	setsid();

	/*
	 * Ensure future opens won't allocate controlling TTYs.
	 */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		err_quit("%s: can't ignore SIGHUP", cmd);
	if ((pid = fork()) < 0)
		err_quit("%s: can't fork", cmd);
	else if (pid != 0) /* parent */
		exit(0);

	/*
	 * Change the current working directory to the root so
	 * we won't prevent file systems from being unmounted.
	 */
	if (chdir("/") < 0)
		err_quit("%s: can't change directory to /", cmd);

	/*
	 * Close all open file descriptors.
	 */
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);
	
	/*
	*  redirect stdin, stdout, stderr
	*/
	snprintf(logfile,  sizeof(logfile), "%s.%d", "/var/log/apue/daemon_log", getpid());	
	fd0 = open("/dev/null", O_RDWR|O_CREAT);
	fd1 = open(logfile, O_RDWR|O_CREAT, LOGMODE);
	fd2 = dup(fd1);
	
	if (fd0 != 0 || fd1 != 1 || fd2 != 2)
		err_quit("%s: can't freopen fd1", cmd);

	setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
	setvbuf(stderr, NULL, _IOLBF, BUFSIZ);
}


int already_running(void)
{
	int		fd;
	char	buf[16];

	fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
	if (fd < 0) 
	{
		printf("can't open %s: %s \n", LOCKFILE, strerror(errno));
		exit(1);
	}
	if (lockfile(fd) < 0) 
	{
		if (errno == EACCES || errno == EAGAIN) 
		{		
			printf("already lock by process:%d \n", lock_test(fd, F_WRLCK, 0, SEEK_SET, 0));		
			close(fd);
			return(1);
		}
		printf("can't lock %s: %s", LOCKFILE, strerror(errno));
		exit(1);
	}
	ftruncate(fd, 0);
	sprintf(buf, "%ld", (long)getpid());
	write(fd, buf, strlen(buf)+1);
	return(0);
}

void * thr_fn(void *arg)
{
	int err, signo;

	for (;;) 
	{
		err = sigwait(&mask, &signo);
		if (err != 0) 
		{
			printf("sigwait failed \n");
			exit(1);
		}

		switch (signo) 
		{
			case SIGHUP:
				printf("Re-reading configuration file \n");
				break;

			case SIGTERM:
				printf("got SIGTERM; exiting \n");
				exit(0);

			default:
				printf("unexpected signal %d\n", signo);
		}
	}
	return(0);
}

int main(int argc, char *argv[])
{
	int					err;
	pthread_t			tid;
	char				*cmd;
	struct sigaction	sa;
	char *login_name = NULL;
	
	if ((cmd = strrchr(argv[0], '/')) == NULL)
		cmd = argv[0];
	else
		cmd++;
	
	daemonize(cmd);	
	printf("test_daemon start \n");
	
	if (already_running()) 
	{
		printf("daemon already running \n");	
		exit(1);
	}	

	if ((login_name = getlogin()) == NULL)
		printf("no login name \n");
	else
		printf("login:%s\n", login_name);

	/*
	 * Restore SIGHUP default and block all signals.
	 */
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		err_quit("%s: can't restore SIGHUP default");
	sigfillset(&mask);
	if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
		err_exit(err, "SIG_BLOCK error");
	
	/*
	 * Create a thread to handle SIGHUP and SIGTERM.
	 */
	err = pthread_create(&tid, NULL, thr_fn, 0);
	if (err != 0)
		err_exit(err, "can't create thread");

	pthread_join(tid, NULL);
	exit(0);
}