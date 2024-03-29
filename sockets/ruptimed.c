#include "my_daemonize.h"
#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <fcntl.h>

#define BUFLEN	128
#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

extern int initserver(int socktype, const struct sockaddr *addr, socklen_t alen, int qlen);

void serve(int sockfd)
{
	int		connect_fd;
	FILE	*fp;
	char	buf[BUFLEN] = {0};

	set_cloexec(sockfd);
	for (;;) 
	{
		if ((connect_fd = accept(sockfd, NULL, NULL)) < 0) 
		{
			printf("ruptimed: accept error: %s", strerror(errno));
			exit(1);
		}
		set_cloexec(connect_fd);
		if ((fp = popen("/usr/bin/uptime", "r")) == NULL) 
		{
			sprintf(buf, "error: %s\n", strerror(errno));
			send(connect_fd, buf, strlen(buf), 0);
		} 
		else 
		{
			while (fgets(buf, BUFLEN, fp) != NULL)
				send(connect_fd, buf, strlen(buf), 0);
			pclose(fp);
		}
		close(connect_fd);
	}
}

int main(int argc, char *argv[])
{
	struct addrinfo	*ailist, *aip;
	struct addrinfo	hint;
	int				sockfd, err, n;
	char			*host;

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
	hint.ai_flags = AI_CANONNAME;
	hint.ai_family = AF_INET;
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
		if ((sockfd = initserver(SOCK_STREAM, aip->ai_addr, aip->ai_addrlen, QLEN)) >= 0) 
		{
			serve(sockfd);
			exit(0);
		}
		else
			printf("init_server failed, %s\n", strerror(errno));
	}
	
	printf("process exit.\n");
	exit(1);
}
