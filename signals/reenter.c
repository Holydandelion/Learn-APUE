#include "apue.h"
#include <pwd.h>

static void
my_alarm(int signo)
{
	struct passwd	*rootptr;

	printf("in signal handler\n");
	if ((rootptr = getpwnam("root")) == NULL)
			err_sys("getpwnam(root) error");
	alarm(1);
}

int
main(void)
{
	struct passwd	*ptr;

	signal(SIGALRM, my_alarm);
	alarm(1);
	for ( ; ; ) {
		if ((ptr = getpwnam("chenruogeng")) == NULL)
			err_sys("getpwnam error");
		
		sleep(1);
		
		if (strcmp(ptr->pw_name, "chenruogeng") != 0)
			printf("return value corrupted!, pw_name = %s\n",
					ptr->pw_name);
	}
}
