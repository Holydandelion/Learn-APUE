
#include "server_unix_sock.h"
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <errno.h>
#include <stddef.h>
#include <sys/stat.h>

#define	STALE	30	/* client's name can't be older than this (sec) */

/*
 * Create a server endpoint of a connection.
 * Returns fd if all OK, <0 on error.
 */
int server_listen(const char *path)
{
	int	fd, len, err, rval;
	struct sockaddr_un	server_addr;

	if (strlen(path) >= sizeof(server_addr.sun_path)) {
		errno = ENAMETOOLONG;
		return(-1);
	}
	
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		return(-2);

	unlink(path);	/* in case it already exists */

	/* fill in socket address structure */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, path);
	len = offsetof(struct sockaddr_un, sun_path) + strlen(path);

	if (bind(fd, (struct sockaddr *)&server_addr, len) < 0) 
	{
		rval = -3;
		goto errout;
	}
	
	/*change permision to allow client processes to connnect*/
	if (chmod(server_addr.sun_path, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH) < 0) {
		rval = -4;
		goto errout;
	}	

	if (listen(fd, LISTEN_QLEN) < 0) 
	{	
		rval = -5;
		goto errout;
	}
	return(fd);

errout:
	err = errno;
	close(fd);
	errno = err;
	return(rval);
}


/*
 * Wait for a client connection to arrive, and accept it.
 * We also obtain the client's user ID from the pathname
 * that it must bind before calling us.
 * Returns new fd if all OK, <0 on error
 */
int server_accept(int listenfd, uid_t *uidptr)
{
	int					client_fd, err, rval;
	socklen_t			len;
	time_t				staletime;
	struct sockaddr_un	client_addr;
	struct stat			statbuf;
	char				*path;

	/* allocate enough space for longest name plus terminating null */
	if ((path = malloc(sizeof(client_addr.sun_path + 1))) == NULL)
		return(-1);
		
	len = sizeof(client_addr);
	if ((client_fd = accept(listenfd, (struct sockaddr *)&client_addr, &len)) < 0) 
	{
		free(path);
		return(-2);		/* often errno=EINTR, if signal caught */
	}

	/* obtain the client's uid from its calling address */
	len -= offsetof(struct sockaddr_un, sun_path); /* len of pathname */
	memcpy(path, client_addr.sun_path, len);
	path[len] = 0;			/* null terminate */
	if (stat(path, &statbuf) < 0) 
	{
		rval = -3;
		goto errout;
	}

#ifdef	S_ISSOCK	/* not defined for SVR4 */
	if (S_ISSOCK(statbuf.st_mode) == 0) {
		rval = -4;		/* not a socket */
		goto errout;
	}
#endif

	if ((statbuf.st_mode & (S_IRWXG | S_IRWXO)) || (statbuf.st_mode & S_IRWXU) != S_IRWXU) 
	{
		  rval = -5;	/* is not rwx------ */
		  goto errout;
	}

	staletime = time(NULL) - STALE;
	if (statbuf.st_atime < staletime ||
		statbuf.st_ctime < staletime ||
		statbuf.st_mtime < staletime) 
	{
		  rval = -6;	/* i-node is too old */
		  goto errout;
	}

	if (uidptr != NULL)
		*uidptr = statbuf.st_uid;	/* return uid of caller */
	unlink(path);		/* we're done with pathname now */
	free(path);
	return(client_fd);

errout:
	err = errno;
	close(client_fd);
	free(path);
	errno = err;
	return(rval);
}