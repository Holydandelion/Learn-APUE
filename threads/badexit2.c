#include "apue.h"
#include <pthread.h>

struct foo {
	int a, b, c, d;
};

void
printfoo(const char *s, const struct foo *fp)
{
	printf("%s", s);
	printf("  structure at 0x%lx\n", (unsigned long)fp);
	printf("  foo.a = %d\n", fp->a);
	printf("  foo.b = %d\n", fp->b);
	printf("  foo.c = %d\n", fp->c);
	printf("  foo.d = %d\n", fp->d);
}

void *
thr_fn1(void *arg)
{
	struct foo	*foo_ptr= NULL;
	foo_ptr = (struct foo *)malloc(sizeof(struct foo));
	
	if (foo_ptr == NULL)
		return ((void *)NULL);
	
	foo_ptr->a = 1;
	foo_ptr->b = 2;
	foo_ptr->c = 3;
	foo_ptr->d = 4;
	printfoo("thread 1:\n", foo_ptr);
	pthread_exit((void *)foo_ptr);
}

void *
thr_fn2(void *arg)
{
	printf("thread 2: ID is %lu\n", (unsigned long)pthread_self());
	pthread_exit((void *)0);
}

int
main(void)
{
	int			err;
	pthread_t	tid1, tid2;
	struct foo	*fp;

	err = pthread_create(&tid1, NULL, thr_fn1, NULL);
	if (err != 0)
		err_exit(err, "can't create thread 1");
	err = pthread_join(tid1, (void *)&fp);
	if (err != 0)
		err_exit(err, "can't join with thread 1");
	sleep(1);
	printf("parent starting second thread\n");
	err = pthread_create(&tid2, NULL, thr_fn2, NULL);
	if (err != 0)
		err_exit(err, "can't create thread 2");
	sleep(1);
	
	if (fp != NULL)
	{
		printfoo("parent:\n", fp);
		free(fp);
	}
	exit(0);
}
