#include "thread_pool.h"

#define TASK_QUEUE_SIZE (100)
#define NSEC_PER_SEC    1000000000

#define QUEUE_IS_FULL(queue) (queue->head == (queue->tail + 1) % TASK_QUEUE_SIZE)
#define QUEUE_POINT_INC(point) ((point + 1) % TASK_QUEUE_SIZE)

static thread_pool_t *g_thread_pool;

static void *worker_thread_func(void *arg) 
{
    
    worker_func_t func;
    void *arg;
    while (!g_thread_pool->terminate) {     
        rv = pthread_cond_timedwait(&g_thread_pool->notify, &g_thread_pool->lock, NSEC_PER_SEC/5);
        if (rv != 0) {
            pthread_mutex_unlock(&g_thread_pool->lock);
            if (rv != ETIMEDOUT) {
                errno = rv;
                log_perror("E00034:\tpthread_cond_timedwait() failed....");
            }
            return rv;
        }

        func = task_queue->queue[task_queue->current].func;
        arg = task_queue->queue[task_queue->current].arg;

        task_queue->queue[task_queue->current].func = NULL;
        task_queue->queue[task_queue->current].arg = NULL;
        task_queue->head = QUEUE_POINT_INC(task_queue->head);

        pthread_mutex_unlock(&g_thread_pool->lock);
        
        //调用回调函数
        (*func)(arg);
    }
}

int init_thread_pool(int thread_num)
{
    g_thread_pool = (thread_pool_t *)malloc(sizeof(thread_pool_t));
    if (g_thread_pool == NULL)  {
        fprintf(stderr, "malloc thread_pool_t error!!!!\n");
        return -1;
    }
    memset(g_thread_pool, 0, sizeof(sizeof(thread_pool_t)));

    g_thread_pool->thread_ids = (pthread_t **)malloc(sizeof(pthread_t *) * thread_num);
    if (g_thread_pool->thread_ids == NULL) {
        fprintf(stderr, "malloc thread ids error!!!!\n");
        return -1;
    }
    
    g_thread_pool->task_queue = (task_queue_t *)malloc(sizeof(task_queue_t));
    if (g_thread_pool->task_queue == NULL) {
        fprintf(stderr, "malloc task queue error!!!!\n");
        return -1;
    }
    
    g_thread_pool->task_queue->queue = (thread_task_t *)malloc(sizeof(thread_task_t) * TASK_QUEUE_SIZE);
    if (g_thread_pool->task_queue->queue == NULL) {
        fprintf(stderr, "malloc task queue error!!!!\n");
        return -1;
    }

    for (int i = 0; i < g_thread_pool->thread_num; ++i) {
        ret = pthread_create(&g_thread_pool->thread_ids[i], NULL,
                worker_thread_func, NULL);
        if (ret != 0) {
            fprintf(stderr, "pthread_create error!!!!\n");
            return -2;
        }
        g_thread_pool->thread_num++;
        g_thread_pool->started++;
    }

    g_thread_pool->task_queue->head = 0;
    g_thread_pool->task_queue->tail = 0;
    return 0;
}

int thread_pool_add(thread_pool_t *thread_pool, worker_func_t func, void *arg)
{
    task_queue_t *task_queue = thread_pool->task_queue;

    pthread_mutex_lock(&thread_pool->lock);
    if (QUEUE_IS_FULL(task_queue)) {
        fprintf(stderr, "task queue is full error!!!!\n");
        pthread_mutex_unlock(thread_pool->lock);
        return -1;
    }
    
    task_queue->queue[task_queue->current].func = func;
    task_queue->queue[task_queue->current].arg = arg;
    task_queue->tail = QUEUE_POINT_INC(task_queue->tail);
    pthread_cond_signal(&thread_pool->notify);
    pthread_mutex_unlock(&thread_pool->lock);
    return 0;
}

//TODO：销毁线程池
//TODO：挂起线程池
