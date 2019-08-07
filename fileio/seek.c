#include "apue.h"
#include <errno.h>
#include <string.h>

int
main(void)
{
	if (lseek(STDIN_FILENO, 0, SEEK_CUR) == -1)
		printf("cannot seek: %s\n", strerror(errno));
	else
		printf("seek OK\n");
	exit(0);
}
