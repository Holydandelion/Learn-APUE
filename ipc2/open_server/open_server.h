
#ifndef OPEN_SERVER_H
#define OPEN_SERVER_H
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 4096
#define	SERVER_PATH "/tmp/opend.socket"	/* well-known name */
#define	REQUEST "open"				/* client's request for server */

extern int	 debug;		/* nonzero if interactive (not daemon) */
extern int	 oflag;		/* open flag: O_xxx ... */
extern char	 errmsg[];	/* error message string to return to client */
extern char	*pathname;	/* of file to open for client */


typedef struct 
{	
	/* one Client struct per connected client */
	int	fd;			/* fd, or -1 if available */
	uid_t	uid;
} Client;

extern Client	*client_array;		/* ptr to malloc'ed array */
extern int		 client_size;	/* # entries in client[] array */

int client_add(int fd, uid_t uid);
void client_del(int fd);
void loop(void);
#endif
