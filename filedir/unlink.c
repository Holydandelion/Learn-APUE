#include "apue.h"
#include <fcntl.h>

int
main(void)
{
	char buf[24] = {0};
	char content[]="0123456879";
	int fd = -1;
	ssize_t read_num = 0;	

	if ((fd = open("tempfile", O_RDWR|O_CREAT)) < 0)
		err_sys("open error");
		
	if (unlink("tempfile") < 0)
		err_sys("unlink error");		
	printf("file unlinked\n");
	
	if (write(fd, content, sizeof(content)) != sizeof(content))
		err_sys("write error");	
	
	printf("[DEBUG]write bytes:%ld \n", sizeof(content));

	if ((read_num = read(fd, buf, sizeof(buf)))< 0)
		err_sys("read error");	
	
	printf("[DEBUG]read_num:%d, read from file:%s \n", read_num, buf);
					
	sleep(15);
	printf("done\n");
	exit(0);
}
