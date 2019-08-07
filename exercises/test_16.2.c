

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

void print_stat(struct stat *stat_p)
{
	if (stat_p == NULL)
		return;
		
	printf("file type:%s\n", S_ISSOCK(stat_p->st_mode)?"socket":"others");
	printf("file mode:%o\n", stat_p->st_mode);	
	printf("inode number:%ld\n", (long)stat_p->st_ino);
	printf("device number:%ld\n", (long)stat_p->st_dev);	
	printf("special device number:%ld\n", (long)stat_p->st_rdev);	
	printf("number of links:%ld\n", (long)stat_p->st_nlink);			
	printf("uid:%d\n", stat_p->st_uid);
	printf("gid:%d\n", stat_p->st_gid);
	printf("size:%ld\n", stat_p->st_size);	
	printf("last access time:%s\n", ctime(&stat_p->st_atime));
	printf("last modify time:%s\n", ctime(&stat_p->st_mtime));	
	printf("last change time:%s\n", ctime(&stat_p->st_ctime));		
	printf("block size:%ld\n", (long)stat_p->st_blksize);
	printf("number of block:%ld\n", (long)stat_p->st_blocks);
}

int main(void)
{
	int sockfd = -1;
	struct stat fd_stat;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error, %s\n", strerror(errno));
		exit(1);
	}
	
	memset(&fd_stat, 0x00, sizeof(fd_stat));
	if (fstat(sockfd, &fd_stat) < 0)
	{
		printf("fstat error, %s\n", strerror(errno));
		exit(1);
	}	
	
	print_stat(&fd_stat);
	close(sockfd);
	exit(0);
}