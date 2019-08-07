#ifndef _WRITEN_H
#define _WRITEN_H
#include <unistd.h>

ssize_t writen(int fd, const void *ptr, size_t n);
#endif
