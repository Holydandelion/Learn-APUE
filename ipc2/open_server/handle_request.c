#include	"open_server.h"
#include 	"writen.h"
#include 	<sys/socket.h>
#include	"handle_request.h"
#include	"buf_args.h"

/* size of control buffer to send/recv one file descriptor */
#define	CONTROLLEN	CMSG_LEN(sizeof(int))

static struct cmsghdr	*cmptr = NULL;	/* malloc'ed first time */

static int send_err(int fd, int errcode, const char *msg);
static int send_fd(int fd, int fd_to_send);
static int client_args(int argc, char **argv);

/*
 * This function is called by buf_args(), which is called by
 * handle_request().  buf_args() has broken up the client's
 * buffer into an argv[] style array, which we now process.
 */
int client_args(int argc, char **argv)
{
	if (argc != 3 || strcmp(argv[0], REQUEST) != 0) 
	{
		strcpy(errmsg, "usage: <pathname> <oflag>\n");
		return(-1);
	}
	pathname = argv[1];		/* save ptr to pathname to open */
	oflag = atoi(argv[2]);
	return(0);
}


/*
 * Used when we had planned to send an fd using send_fd(),
 * but encountered an error instead.  We send the error back
 * using the send_fd()/recv_fd() protocol.
 */
int send_err(int fd, int errcode, const char *msg)
{
	int		n;

	if ((n = strlen(msg)) > 0)
		if (writen(fd, msg, n) != n)	/* send the error message */
			return(-1);

	if (errcode >= 0)
		errcode = -1;	/* must be negative */

	if (send_fd(fd, errcode) < 0)
		return(-1);

	return(0);
}

/*
 * Pass a file descriptor to another process.
 * If fd<0, then -fd is sent back instead as the error status.
 */
int send_fd(int fd, int fd_to_send)
{
	struct iovec	iov[1];
	struct msghdr	msg;
	char			buf[2];	/* send_fd()/recv_fd() 2-byte protocol */

	iov[0].iov_base = buf;
	iov[0].iov_len  = 2;
	msg.msg_iov     = iov;
	msg.msg_iovlen  = 1;
	msg.msg_name    = NULL;
	msg.msg_namelen = 0;

	if (fd_to_send < 0) 
	{
		msg.msg_control    = NULL;
		msg.msg_controllen = 0;
		buf[1] = -fd_to_send;	/* nonzero status means error */
		if (buf[1] == 0)
			buf[1] = 1;	/* -256, etc. would screw up protocol */
	} else 
	{
		if (cmptr == NULL && (cmptr = malloc(CONTROLLEN)) == NULL)
			return(-1);
			
		cmptr->cmsg_level  = SOL_SOCKET;
		cmptr->cmsg_type   = SCM_RIGHTS;
		cmptr->cmsg_len    = CONTROLLEN;
		msg.msg_control    = cmptr;
		msg.msg_controllen = CONTROLLEN;
		*(int *)CMSG_DATA(cmptr) = fd_to_send;		/* the fd to pass */
		buf[1] = 0;		/* zero status means OK */
	}

	buf[0] = 0;			/* null byte flag to recv_fd() */
	if (sendmsg(fd, &msg, 0) != 2)
		return(-1);
	return(0);
}



void handle_request(char *buf, size_t size, int fd, uid_t uid)
{
	int		new_fd;

	if (buf[size-1] != 0) 
	{
		snprintf(errmsg, MAXLINE - 1, "request from uid %d not null terminated: %*.*s\n", uid, size, size, buf);
		send_err(fd, -1, errmsg);
		return;
	}
	printf("request: %s, from uid %d \n", buf, uid);

	/* parse the arguments, set options */
	if (buf_args(buf, client_args) < 0) 
	{
		send_err(fd, -1, errmsg);
		printf("%s\n", errmsg);
		return;
	}

	if ((new_fd = open(pathname, oflag)) < 0) 
	{
		snprintf(errmsg, MAXLINE-1, "can't open %s: %s\n", pathname, strerror(errno));
		send_err(fd, -1, errmsg);
		printf("%s\n", errmsg);
		return;
	}

	/* send the descriptor */
	if (send_fd(fd, new_fd) < 0)
	{
		printf("send_fd error, %s", strerror(errno));
		exit(1);
	}
		
	printf("sent fd %d over fd %d for %s \n", new_fd, fd, pathname);
	close(new_fd);		/* we're done with descriptor */
}