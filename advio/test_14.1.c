

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int lock_file(int fd, int type, int whence, off_t start, off_t len);

int main(int argc, char *argv[])
{
	int fd = -1;
	int n = 0;
	char buf[12] = {0};
	if (argc != 2)
	{
		printf("argc must be 2\n");
		exit(1);
	}
	
	if ((fd = open("./testfile", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0)
	{
		printf("open failed, %s\n", strerror(errno));
		exit(1);
	}
	
	if (strcmp(argv[1], "F_RDLCK") == 0)
	{
		if (lock_file(fd, F_RDLCK, SEEK_SET, 0, 10) < 0)
		{
			printf("lock_file failed, %s\n", strerror(errno));
			exit(1);
		}
		
		if ((n = read(fd, buf, 10)) != 10)
		{
			printf("read failed, %s\n", strerror(errno));
			exit(1);
		}	
		printf("read from file:%s \n", buf);	
		sleep(10);
	}
	else if (strcmp(argv[1], "F_WRLCK") == 0)
	{
		if (lock_file(fd, F_WRLCK, SEEK_SET, 0, 10) < 0)
		{
			printf("lock_file failed, %s\n", strerror(errno));
			exit(1);
		}
		printf("lock file with cmd F_WRLCK. \n");		
	}
	else
	{
		printf("Not supported.\n");
	}
	
	close(fd);
	exit(0);
}

int lock_file(int fd, int type, int whence, off_t start, off_t len)
{
	struct flock lock_tmp, lock_2;
	
	memset(&lock_tmp, 0x00, sizeof(lock_tmp));
	
	lock_tmp.l_type = type;
	lock_tmp.l_whence = whence;
	lock_tmp.l_start = start;
	lock_tmp.l_len = len;
	
	lock_2 = lock_tmp;
	return (fcntl(fd, F_SETLKW, &lock_2));  
}