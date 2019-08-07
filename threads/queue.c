#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define JOB_CONTENT_SIZE 256
#define NTHREAD 3


struct job {
	struct job *j_next;
	struct job *j_prev;
	pthread_t   j_id;   /* tells which thread handles this job */
	/* ... more stuff here ... */
	char	j_content[JOB_CONTENT_SIZE];
};

struct queue {
	struct job      *q_head;
	struct job      *q_tail;
	pthread_rwlock_t q_lock;
	pthread_cond_t 	 q_cond[NTHREAD];
	pthread_mutex_t  q_mutex[NTHREAD];
	pthread_t 		 q_thread[NTHREAD];
};

/*
 * Initialize a queue.
 */
int queue_init(struct queue *qp)
{
	int err;
	int i = 0;
	
	qp->q_head = NULL;
	qp->q_tail = NULL;
	err = pthread_rwlock_init(&qp->q_lock, NULL);
	if (err != 0)
		return(err);
	
	for (i=0;i<NTHREAD;i++)
	{	
		err = pthread_mutex_init(&qp->q_mutex[i], NULL);
		if (err != 0)
			return(err);	
		
		err = pthread_cond_init(&qp->q_cond[i], NULL);
		if (err != 0)
			return(err);
	}	
		
	/* ... continue initialization ... */
	return(0);
}

/*
 * Insert a job at the head of the queue.
 */
int job_insert(struct queue *qp, struct job *jp)
{
	int i = 0;
	struct job *job_ptr = NULL;
	job_ptr = (struct job *)malloc(sizeof(struct job));
	if (job_ptr == NULL)
		return -1;
	
	memcpy(job_ptr, jp, sizeof(struct job));
	
	for (i=0;i<NTHREAD;i++)
	{	
		if (pthread_equal(qp->q_thread[i], jp->j_id))
			break;
	}
	if (i == NTHREAD)
		return -1;
	
	pthread_mutex_lock(&qp->q_mutex[i]);		
	pthread_rwlock_wrlock(&qp->q_lock);
	job_ptr->j_next = qp->q_head;
	job_ptr->j_prev = NULL;
	if (qp->q_head != NULL)
		qp->q_head->j_prev = job_ptr;
	else
		qp->q_tail = job_ptr;	/* list was empty */
	qp->q_head = job_ptr;
	pthread_rwlock_unlock(&qp->q_lock);
	pthread_mutex_unlock(&qp->q_mutex[i]);		
	pthread_cond_signal(&qp->q_cond[i]);			
	return 0;
}

/*
 * Append a job on the tail of the queue.
 */
int job_append(struct queue *qp, struct job *jp)
{
	int i = 0;
	struct job *job_ptr = NULL;
	
	job_ptr = (struct job *)malloc(sizeof(struct job));
	if (job_ptr == NULL)
		return -1;
	
	memcpy(job_ptr, jp, sizeof(struct job));	
	for (i=0;i<NTHREAD;i++)
	{	
		if (pthread_equal(qp->q_thread[i], jp->j_id))
			break;
	}
	if (i == NTHREAD)
		return -1;
	
	pthread_mutex_lock(&qp->q_mutex[i]);		
	pthread_rwlock_wrlock(&qp->q_lock);
	job_ptr->j_next = NULL;
	job_ptr->j_prev = qp->q_tail;
	if (qp->q_tail != NULL)
		qp->q_tail->j_next = job_ptr;
	else
		qp->q_head = job_ptr;	/* list was empty */
	qp->q_tail = job_ptr;
	
	pthread_rwlock_unlock(&qp->q_lock);
	pthread_mutex_unlock(&qp->q_mutex[i]);
	pthread_cond_signal(&qp->q_cond[i]);	
	return 0;
}

/*
 * Remove the given job from a queue.
 */
void
job_remove(struct queue *qp, struct job *jp)
{
	pthread_rwlock_wrlock(&qp->q_lock);
	if (jp == qp->q_head) {
		qp->q_head = jp->j_next;
		if (qp->q_tail == jp)
			qp->q_tail = NULL;
		else
			jp->j_next->j_prev = jp->j_prev;
	} else if (jp == qp->q_tail) {
		qp->q_tail = jp->j_prev;
		jp->j_prev->j_next = jp->j_next;
	} else {
		jp->j_prev->j_next = jp->j_next;
		jp->j_next->j_prev = jp->j_prev;
	}
	free(jp);	
	pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Find a job for the given thread ID.
 */
struct job *job_find(struct queue *qp, pthread_t id)
{
	struct job *jp;

	if (pthread_rwlock_rdlock(&qp->q_lock) != 0)
		return(NULL);

	for (jp = qp->q_head; jp != NULL; jp = jp->j_next)
		if (pthread_equal(jp->j_id, id))
			break;

	pthread_rwlock_unlock(&qp->q_lock);
	return(jp);
}

void *handle_job(void *arg)
{
	int i = 0;
	struct job *job_ptr = NULL;
	struct queue *queue_ptr = (struct queue *)arg;
	
	for (i=0;i<NTHREAD;i++)
	{
		if (pthread_equal(queue_ptr->q_thread[i], pthread_self()))
			break;
	}
	if (i == NTHREAD)
		return ((void *)0);
	
	while(1)
	{
		pthread_mutex_lock(&queue_ptr->q_mutex[i]);	
		while (queue_ptr->q_tail == NULL || queue_ptr->q_head == NULL)  /* check the condition again, it may change*/
		{
			printf("[DEBUG]thread:%lx, the queue is empty\n ",pthread_self());
			pthread_cond_wait(&queue_ptr->q_cond[i], &queue_ptr->q_mutex[i]);	
			printf("[DEBUG]thread:%lx, get the condition\n ",pthread_self());	
		}	
		
		job_ptr = job_find(queue_ptr, pthread_self());	
		if (job_ptr != NULL)
		{
			printf("[INFO]job done, thread_id:%lx, job content: %s \n", job_ptr->j_id, job_ptr->j_content);
			job_remove(queue_ptr, job_ptr);		
		}	
		else
		{
			printf("[INFO]job not found, thread_id:%lx.\n", pthread_self()); 
			/* avoid occupy the cpu*/
			pthread_mutex_unlock(&queue_ptr->q_mutex[i]);				
			pthread_mutex_lock(&queue_ptr->q_mutex[i]);
			pthread_cond_wait(&queue_ptr->q_cond[i], &queue_ptr->q_mutex[i]);	
		}
		pthread_mutex_unlock(&queue_ptr->q_mutex[i]);			

	}
	return ((void *)0);
}


int main(void)
{
	int ret = -1;
	int i = 0;
	char buffer[JOB_CONTENT_SIZE] = {0};
	char c = 0;
	struct job	job_tmp;
	struct queue *queue_ptr = NULL;
		
	setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
	queue_ptr = (struct queue *)malloc(sizeof(struct queue));
	if (queue_ptr == NULL)
		exit(1);

	ret = queue_init(queue_ptr);
	if (ret != 0)
	{
		printf("[ERROR]queue_init failed, %d \n", ret);
		exit(1);
	}	
	
	for (i=0;i<NTHREAD;i++)
	{	
		ret = pthread_create(&queue_ptr->q_thread[i], NULL, handle_job, (void *)queue_ptr);
		if (ret != 0)
		{
			printf("[ERROR]pthread_create failed, %d \n", ret);
			exit(1);
		}
	}
	
	while (fgets(buffer, sizeof(buffer), stdin))
	{
		c = buffer[0];
		i = c - '0';
		if ( i >= 1 && i <= NTHREAD)			
		{
			memcpy(&job_tmp.j_id, &queue_ptr->q_thread[i-1], sizeof(pthread_t));
			strcpy(job_tmp.j_content, buffer);
		}
		else
		{
			printf("Not supported id \n");
			continue;
		}
		job_append(queue_ptr, &job_tmp);	
		printf("job append, job id:%lx\n", job_tmp.j_id);

	}

	for (i=0;i<NTHREAD;i++)
	{	
		ret = pthread_join(queue_ptr->q_thread[i], NULL);
		if (ret != 0)
		{
			printf("[ERROR]pthread_join failed, %d \n", ret);
			exit(1);
		}
	}
	
	free(queue_ptr); /* the process exit, all resource will be auto released.*/
	exit(0);
}   

