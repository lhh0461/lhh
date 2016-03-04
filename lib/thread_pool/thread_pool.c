#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "thread_pool.h"


#define TASK_QUEUE_SIZE (100)
#define NSEC_PER_SEC    1000000000

#define QUEUE_IS_EMPTY(queue) (queue->head == queue->tail)

#define QUEUE_IS_FULL(queue) (queue->head == (queue->tail + 1) % TASK_QUEUE_SIZE)

#define QUEUE_ADD_POS(pos) ((pos) = ((pos) + 1) % TASK_QUEUE_SIZE)

static thread_pool_t *g_thread_pool;
static int g_terminate;

static void *worker_thread_func(void *arg) 
{
    thread_pool_t *pool = (thread_pool_t *)arg;
    task_queue_t *queue = pool->queue;
    worker_func_t func;
    void *args;
    int rv;
    while (!g_terminate) {
        rv = pthread_cond_timedwait(&pool->notify, &pool->lock, NSEC_PER_SEC/5);
        if (rv != 0) {
            pthread_mutex_unlock(&pool->lock);
            if (rv != ETIMEDOUT) {
                errno = rv;
                fprintf(stderr, "E00034:\tpthread_cond_timedwait() error!!!!!\n");
                return (void *)rv;
            }
            continue;
        }
        
        func = queue->tasks[queue->head].func;
        args = queue->tasks[queue->head].arg;

        queue->tasks[queue->head].func = NULL;
        queue->tasks[queue->head].arg = NULL;
        QUEUE_ADD_POS(queue->head);

        pthread_mutex_unlock(&pool->lock);
        
        //调用回调函数
        (*func)(args);
    }
}

/*
 * 初始化线程池
 * thread_num 为线程池的线程数量
 */
int init_thread_pool(int thread_num)
{
    g_thread_pool = (thread_pool_t *)malloc(sizeof(thread_pool_t));
    if (g_thread_pool == NULL)  {
        fprintf(stderr, "malloc thread_pool_t error!!!!\n");
        return -1;
    }
    memset(g_thread_pool, 0, sizeof(thread_pool_t));

    g_thread_pool->pids = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
    if (g_thread_pool->pids == NULL) {
        fprintf(stderr, "malloc thread ids error!!!!\n");
        return -1;
    }
    
    g_thread_pool->queue = (task_queue_t *)malloc(sizeof(task_queue_t));
    if (g_thread_pool->queue == NULL) {
        fprintf(stderr, "malloc task queue error!!!!\n");
        return -1;
    }
    
    g_thread_pool->queue->tasks = (thread_task_t *)malloc(sizeof(thread_task_t) * TASK_QUEUE_SIZE + 1);
    if (g_thread_pool->queue->tasks == NULL) {
        fprintf(stderr, "malloc task queue error!!!!\n");
        return -1;
    }

    int i = 0, ret = 0;
    for (; i < g_thread_pool->thread_num; ++i) {
        ret = pthread_create(&g_thread_pool->pids[i], NULL,
                worker_thread_func, g_thread_pool);
        if (ret != 0) {
            fprintf(stderr, "pthread_create error!!!!\n");
            return -2;
        }
        g_thread_pool->thread_num++;
        g_thread_pool->started++;
    }

    g_thread_pool->queue->head = 0;
    g_thread_pool->queue->tail = 0; 
    return 0;
}

int thread_pool_add(thread_pool_t *thread_pool, worker_func_t func, void *arg)
{
    task_queue_t *task_queue = thread_pool->queue;

    pthread_mutex_lock(&thread_pool->lock);
    if (QUEUE_IS_FULL(task_queue)) {
        fprintf(stderr, "task queue is full error!!!!\n");
        pthread_mutex_unlock(&thread_pool->lock);
        return -1;
    }

    task_queue->tasks[task_queue->tail].func = func;
    task_queue->tasks[task_queue->tail].arg = arg;

    QUEUE_ADD_POS(task_queue->tail);

    pthread_cond_signal(&thread_pool->notify);
    pthread_mutex_unlock(&thread_pool->lock);
    return 0;
}

//TODO：销毁线程池
//TODO：挂起线程池

int main()
{

    return 0;
}
