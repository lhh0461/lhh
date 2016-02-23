#ifndef	__MEM_POOL_H__

#include "list.h"

typedef struct mem_block_s
{
    struct list_head free_list;
    struct list_head used_list;
    int size;
    int free_num;
    int used_num;
    struct list_head list;

}mem_block_t;

typedef struct mem_node_s
{
    mem_block_t *block;
    struct list_head list;
    char data[0];

}mem_node_t;

typedef struct mem_pool_s
{
    struct list_head block_list;
    int block_num;
    int alloc_num;
    //pthread_mutex_t lock;

}mem_pool_t;



mem_pool_t *mem_pool_create(int block_size);
int mem_pool_destroy(mem_pool_t *pool);
mem_block_t *mem_pool_alloc_block(mem_pool_t *pool, int block_size);
static int mem_pool_block_add_node(mem_block_t *block, int node_num);
void *mem_pool_malloc(mem_pool_t *pool, int size);
void mem_pool_free(mem_pool_t *pool, void *p);
void dump_mem_pool(mem_pool_t *pool);


#define	__MEM_POOL_H__
#endif	/* __MEM_POOL_H__ */
