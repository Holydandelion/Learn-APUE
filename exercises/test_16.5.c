#include "my_daemonize.h"
#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#define BUFLEN	128
#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

typedef struct fd_list
{
	int fl_sockfd;
	struct fd_list *fl_next;
}fd_list; 

int init_server(int socktype, const struct sockaddr *addr, socklen_t alen, int qlen)
{
	int fd;
	int err = 0;
	int val = 1;

	if ((fd = socket(addr->sa_family, socktype, 0)) < 0)
		return(-1);
				
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, (socklen_t)sizeof(val)) < 0)
		goto errout;
		
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &val, (socklen_t)sizeof(val)) < 0)
		goto errout;		
		
	if (bind(fd, addr, alen) < 0)
		goto errout;
		
	if (socktype == SOCK_STREAM || socktype == SOCK_SEQPACKET) 
	{
		if (listen(fd, qlen) < 0)
			goto errout;
	}
	return(fd);

errout:
	err = errno;
	close(fd);
	errno = err;
	return(-1);
}


void set_fl(int fd, int flags) /* flags are file status flags to turn on */
{
	int		val;

	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
		err_sys("fcntl F_GETFL error");

	val |= flags;		/* turn on flags */

	if (fcntl(fd, F_SETFL, val) < 0)
		err_sys("fcntl F_SETFL error");
}

void serve(int sockfd)
{
	int		connect_fd = -1;
	pid_t pid = -1;

	set_cloexec(sockfd);
	set_fl(sockfd, O_NONBLOCK);

	while ((connect_fd = accept(sockfd, NULL, NULL)) != -1)
	{
		if ((pid=fork()) < 0)
		{
			printf("fork error, %s\n", strerror(errno));
			exit(1);
		}	
		
		if (pid == 0)  /* child */
		{		
			set_cloexec(connect_fd);
			if ((dup2(connect_fd, STDOUT_FILENO) != STDOUT_FILENO) || (dup2(connect_fd, STDERR_FILENO) != STDERR_FILENO))
			{
				printf("fork error, %s\n", strerror(errno));
				exit(1);
			}
			close(connect_fd);
			close(sockfd);
			execl("/usr/bin/uptime", "uptime", (char *)0);
			printf("unexpted returned from exec:uptime, %s\n", strerror(errno));
		}
		else  /* parent */
		{
			close(connect_fd);			
		}
			
	}
}

void handle_sigchld(int signo)
{
	while (waitpid((pid_t)-1, NULL, WNOHANG) > 0)
		;
}

int main(int argc, char *argv[])
{
	struct addrinfo	*ailist, *aip;
	struct addrinfo	hint;
	int				sockfd, err, n;
	char			*host;
	fd_list *fd_list_p = NULL;
	fd_list *fd_temp = NULL;
	fd_list *fd_new = NULL;	
	
	fd_set read_fds;
	int max_fd = 0;
	
	struct sigaction act;
	
	memset(&act, 0x00, sizeof(act));
	act.sa_handler = handle_sigchld;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(SIGCHLD, &act, NULL) < 0)
		err_sys("sigaction error");	

	if (argc != 1)
		err_quit("usage: ruptimed");
		
	if ((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
		n = HOST_NAME_MAX;	/* best guess */
		
	if ((host = malloc(n)) == NULL)
		err_sys("malloc error");
		
	if (gethostname(host, n) < 0)
		err_sys("gethostname error");
		
	my_daemonize("ruptimed");
	
	memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_ALL;
	hint.ai_family = 0;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_protocol = 0;
	hint.ai_addrlen = 0;
	hint.ai_addr = NULL;	
	hint.ai_canonname = NULL;
	hint.ai_next = NULL;
	if ((err = getaddrinfo(host, "ruptime", &hint, &ailist)) != 0) 
	{
		printf("ruptimed: getaddrinfo error: %s", gai_strerror(err));
		exit(1);
	}
	for (aip = ailist; aip != NULL; aip = aip->ai_next) 
	{
		printf("[DEBUG]address family:%d, addr len:%d \n", aip->ai_addr->sa_family, aip->ai_addrlen);
		if ((sockfd = init_server(SOCK_STREAM, aip->ai_addr, aip->ai_addrlen, QLEN)) >= 0) 
		{
			if ((fd_new = (fd_list *)malloc(sizeof(fd_list))) == NULL)
				err_sys("malloc error");
				
			fd_new->fl_sockfd = sockfd;
			fd_new->fl_next = NULL;
			
			if (fd_list_p == NULL)
				fd_list_p = fd_new;
			else
			{
				fd_temp = fd_list_p;
				while(fd_temp->fl_next != NULL)
					fd_temp = fd_temp->fl_next;

				fd_temp->fl_next = fd_new;	
			}
			
			printf("[DEBUG] new listen fd:%d \n",  sockfd);
		}
		else
			printf("init_server failed, %s\n", strerror(errno));
	}
	
	while (fd_list_p != NULL)
	{
		FD_ZERO(&read_fds);
		max_fd = 0;
		for(fd_temp=fd_list_p;fd_temp!=NULL;fd_temp=fd_temp->fl_next)
		{
			FD_SET(fd_temp->fl_sockfd, &read_fds);
			if (fd_temp->fl_sockfd > max_fd)
				max_fd = sockfd;			
		}
		
		if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0)
		{
			/* need to handle interrupt by signals*/
			if (errno == EINTR)
				continue;
			else
				err_sys("select error");	
		}			
	
		printf("[DEBUG]select is returned\n");
		for (fd_temp=fd_list_p;fd_temp!=NULL;fd_temp=fd_temp->fl_next)
		{
			if (FD_ISSET(fd_temp->fl_sockfd, &read_fds))
			{			
				printf("[DEBUG]fd:%d is acceptable\n", fd_temp->fl_sockfd);	
				serve(fd_temp->fl_sockfd);
			}
		}	
	}	
	
	printf("process exit.\n");
	exit(1);
}
