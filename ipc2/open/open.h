#include "apue.h"
#include <errno.h>

#define	CLIENT_REQ "open"			/* client's request for server */
#define SERVER_PATH "/tmp/opend.socket"	/* server's well-known name */

int csopen(char *name, int oflag);
