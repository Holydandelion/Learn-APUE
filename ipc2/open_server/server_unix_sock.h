

#ifndef _SERVER_UNIX_SOCK_H
#define _SERVER_UNIX_SOCK_H

#include <unistd.h>


#define LISTEN_QLEN	10
int server_listen(const char *path);
int server_accept(int listenfd, uid_t *uidptr);

#endif  