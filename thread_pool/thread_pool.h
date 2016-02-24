#ifndef	__THREAD_POOL_H__

#include <pthread.h>

typedef (void *)(*worker_func)(void *) worker_func_t;

typedef struct thread_task_s
{
    worker_func_t func;
    void *arg;
} thread_task_t;

typedef struct task_queue_s
{
    thread_task_t *queue;
    int head;
    int tail;
} task_queue_t;

typedef struct thread_pool_s
{
    int thread_num;
    int started;
    int terminate;
    pthread_t **thread_ids;
    pthread_mutex_t lock;
    pthread_cond_t notify;
    task_queue_t *task_queue;
} thread_pool_t;




int init_thread_pool(int thread_num);
int thread_pool_add(thread_pool_t *thread_pool, worker_func_t func);

#define	__THREAD_POOL_H__
#endif	/* __THREAD_POOL_H__ */
