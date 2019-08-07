#include "apue.h"
#include <errno.h>
#include <sys/socket.h>

int initserver(int socktype, const struct sockaddr *addr, socklen_t alen, int qlen)
{
	int fd;
	int err = 0;

	if ((fd = socket(addr->sa_family, socktype, 0)) < 0)
		return(-1);
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
