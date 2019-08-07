#include "apue.h"

int
main(void)
{
	char	buf[MAXLINE];
	size_t size = 0;
	printf("[DEBUG]\n");
	
	if (freopen("/home/chenruogeng/log/stdio_out.log", "a+", stdout) == NULL)
		err_sys("freopen error");		
	
	if (setvbuf(stdout, NULL, _IOLBF, BUFSIZ) != 0)
		err_sys("setvbuf error");	
	
	while (fgets(buf, MAXLINE, stdin) != NULL)
	{
		if ((size = fputs(buf, stdout)) == EOF)
			err_sys("output error");
			
		printf("[DEBUG]size:%ld, strlen(buf):%ld \r\n", size, strlen(buf));
	}
			
	if (ferror(stdin))
		err_sys("input error");
	
	exit(0);
}
