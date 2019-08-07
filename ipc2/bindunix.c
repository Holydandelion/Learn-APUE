#include "apue.h"
#include <sys/socket.h>
#include <sys/un.h>

int
main(void)
{
	int fd;
	struct sockaddr_un un;
	int addr_len = 0;

	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, "/home/chenruogeng/Learning/APUE/apue.3e/ipc2/foo.socket");
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		err_sys("socket failed");

	addr_len = (int)&un.sun_path - (int)&un + strlen(un.sun_path); 
	printf("addr_len:%d\n", addr_len);
	if (bind(fd, (struct sockaddr *)&un, addr_len) < 0)
		err_sys("bind failed");
	printf("UNIX domain socket bound\n");
	exit(0);
}
