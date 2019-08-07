

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
	int fd = -1;
	if (mkfifo("./file.fifo", S_IRUSR|S_IWUSR) < 0)
	{
		printf("mkfifo failed, %s\n", strerror(errno));
		exit(1);
	}
	
	fd = open("./file.fifo", O_RDWR);
	printf("open fifo:[%d]\n", fd);
	exit(0);
}