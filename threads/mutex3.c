#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "apue.h"

#define FOO_BUFF_SIZE 128

#define NHASH 29
#define HASH(id) (((unsigned long)id)%NHASH)

struct foo *fh[NHASH];
pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

struct foo {
	int             f_count; /* protected by hashlock */
	pthread_mutex_t f_lock;
	int             f_id;
	struct foo     *f_next; /* protected by hashlock */
	/* ... more stuff here ... */
};

struct foo *
foo_alloc(int id) /* allocate the object */
{
	struct foo	*fp;
	int			idx;

	if ((fp = malloc(sizeof(struct foo))) != NULL) {
		fp->f_count = 1;
		fp->f_id = id;
		if (pthread_mutex_init(&fp->f_lock, NULL) != 0) {
			free(fp);
			return(NULL);
		}
		idx = HASH(id);
		pthread_mutex_lock(&hashlock);
		fp->f_next = fh[idx];
		fh[idx] = fp;
		pthread_mutex_lock(&fp->f_lock);
		pthread_mutex_unlock(&hashlock);
		/* ... continue initialization ... */
		pthread_mutex_unlock(&fp->f_lock);
	}
	return(fp);
}

void
foo_hold(struct foo *fp) /* add a reference to the object */
{
	pthread_mutex_lock(&hashlock);
	fp->f_count++;
	printf("thread[0x%lx] hold foo object, id:%d\n ", pthread_self(), fp->f_id);
	pthread_mutex_unlock(&hashlock);
}

struct foo *
foo_find(int id) /* find an existing object */
{
	struct foo	*fp;

	pthread_mutex_lock(&hashlock);
	for (fp = fh[HASH(id)]; fp != NULL; fp = fp->f_next) {
		if (fp->f_id == id) {
			fp->f_count++;
			break;
		}
	}
	pthread_mutex_unlock(&hashlock);
	return(fp);
}

void
foo_rele(struct foo *fp) /* release a reference to the object */
{
	struct foo	*tfp;
	int			idx;

	pthread_mutex_lock(&hashlock);
	printf("thread[0x%lx] release foo object, id:%d\n ", pthread_self(), fp->f_id);	
	if (--fp->f_count == 0) { /* last reference, remove from list */
		idx = HASH(fp->f_id);
		tfp = fh[idx];
		if (tfp == fp) {
			fh[idx] = fp->f_next;
		} else {
			while (tfp->f_next != fp)
				tfp = tfp->f_next;
			tfp->f_next = fp->f_next;
		}
		pthread_mutex_unlock(&hashlock);
		pthread_mutex_destroy(&fp->f_lock);
		free(fp);
	} else {
		pthread_mutex_unlock(&hashlock);
	}
}

void *thr_fn1(void *arg)
{
	struct foo *foo_obj = (struct foo *)arg;
	foo_hold(foo_obj);
	foo_rele(foo_obj);	
	return ((void *)0);
}

int main(void)
{
	int		err = -1;
	void 	*tret = NULL;
	pthread_t	tid1, tid2;
	struct foo *foo_obj = NULL;
	
	if ((foo_obj = foo_alloc(1)) == NULL)
		err_exit(err, "can't foo_alloc");	

	printf("thread[0x%lx]\n ", pthread_self());
	err = pthread_create(&tid1, NULL, thr_fn1, (void *)foo_obj);
	if (err != 0)
		err_exit(err, "can't create thread 1");	
		
	err = pthread_create(&tid2, NULL, thr_fn1, (void *)foo_obj);
	if (err != 0)
		err_exit(err, "can't create thread 1");			

	
	
	err = pthread_join(tid1, &tret);
	if (err != 0)
		err_exit(err, "can't join thread 1");	
	err = pthread_join(tid2, &tret);
	if (err != 0)
		err_exit(err, "can't join thread 2");			

	foo_rele(foo_obj);			
	exit(0);
}
