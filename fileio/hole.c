#include "apue.h"
#include <fcntl.h>

char	buf1[] = "abcdefghij";
char	buf2[] = "ABCDEFGHIJ";

int
main(void)
{
	int		fd;
	off_t off_value = 0;
	
	printf("off_t size is:%ld  sizeof(signed long):%ld\n", sizeof(off_t), sizeof(signed long));
	
	if ((fd = creat("file.hole", FILE_MODE)) < 0)
		err_sys("creat error");

	if (write(fd, buf1, 10) != 10)
		err_sys("buf1 write error");
	/* offset now = 10 */

	if (lseek(fd, 16384, SEEK_SET) == -1)
		err_sys("lseek error");
	/* offset now = 16384 */

	if (write(fd, buf2, 10) != 10)
		err_sys("buf2 write error");
	/* offset now = 16394 */
	
	if ((off_value=lseek(fd, 0, SEEK_END)) == -1)
		err_sys("lseek error");		
	
	printf("curruent offset:%ld\n", off_value);

	exit(0);
}
