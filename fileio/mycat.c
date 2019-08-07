#include "apue.h"
#include <fcntl.h>

#define	BUFFSIZE  4096

int
main(int argc, char *argv[])
{
	int		n;
	int 	fd;
	char	buf[BUFFSIZE];
	printf("[DEBUG]:%s \n", argv[1]);
	if ((fd = open(argv[1], O_RDONLY, 0)) == -1)
		err_sys("open error");		
	
	while ((n = read(fd, buf, BUFFSIZE)) > 0)
		if (write(STDOUT_FILENO, buf, n) != n)
			err_sys("write error");
	
	exit(0);
}
