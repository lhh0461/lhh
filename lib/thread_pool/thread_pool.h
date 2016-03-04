#ifndef	__THREAD_POOL_H__
#define	__THREAD_POOL_H__

#include <pthread.h>

typedef void *(*worker_func_t)(void *);

//任务结构体
typedef struct thread_task_s
{
    worker_func_t func;
    void *arg;
} thread_task_t;

//管理任务的循环队列
typedef struct task_queue_s
{
    thread_task_t *tasks;
    int head;//若队列不空，指向头元素
    int tail;//若队列不空，指向队列尾元素的下一元素 
} task_queue_t;

//线程池结构体
typedef struct thread_pool_s
{
    int thread_num;
    int started;
    pthread_t *pids;
    pthread_mutex_t lock;
    pthread_cond_t notify;
    task_queue_t *queue;
} thread_pool_t;




int init_thread_pool(int thread_num);
int thread_pool_add(thread_pool_t *thread_pool, worker_func_t func, void *arg);

#endif	/* __THREAD_POOL_H__ */
