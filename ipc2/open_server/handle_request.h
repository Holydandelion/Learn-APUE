#ifndef _HANDLE_REQUEST_H_
#define _HANDLE_REQUEST_H_
#include	<fcntl.h>

void handle_request(char *buf, size_t size, int fd, uid_t uid);
#endif