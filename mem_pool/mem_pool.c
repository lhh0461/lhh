#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem_pool.h"

#define BLOCK_NODE_NUM 10
#define FOREACH_BLOCK_LIST(pos, head) \
    for (pos = head; pos != NULL; pos = pos->next) {

mem_pool_t * mem_pool_create(int elem_size, int elem_num)
{
    mem_pool_t *pool = NULL;
    pool = (mem_pool_t *)malloc(sizeof(mem_pool_t));
    if (pool == NULL) {
        fprintf(stderr, "malloc pool error!!!!\n");
        return NULL;
    }
    memset(pool, 0, sizeof(mem_pool_t));
    pool->elem_size = elem_size;
    pool->elem_num = elem_num;

    mem_block_t *blk = __mem_pool_alloc_block(pool, elem_size, elem_num); 
    pool->block_list = blk;
    
    fprintf(stdout, "create mem_pool success.\n");
    return pool;
}

int mem_pool_destroy(mem_pool_t *pool)
{
    if (pool == NULL) {
        fprintf(stderr, "pool is NULL!!!!\n");
        return -1;
    }
     
    mem_block_t *block = NULL;
    mem_node_t *node = NULL;
    struct list_head *pos, *used_pos, *free_pos;

    list_for_each (pos, &pool->block_list) {
        block = list_entry(pos, struct mem_block_s, list); 
        
        list_for_each (used_pos, &block->used_list) {
            node = list_entry(used_pos, struct mem_node_s, list); 
            list_del(&node->list);
            free(node);
        }
        
        list_for_each (free_pos, &block->free_list) {
            node = list_entry(free_pos, struct mem_node_s, list); 
            list_del(&node->list);
            free(node);
        }

        list_del(&block->list);
        free(block);
    }
    free(pool);
    fprintf(stdout, "destory mem_pool success.\n");
    return 0;
}

static inline mem_block_t *__mem_pool_alloc_block(mem_pool_t *pool, int elem_size, int elem_num)
{
    if (pool == NULL) {
        return 0;
    }
    mem_block_t *block = NULL;
    block = (mem_block_t *)malloc(sizeof(struct mem_block_s));
    if (block == NULL) {
        fprintf(stderr, "malloc block error!!!!\n");
        return NULL;
    }
    memset(block, 0, sizeof(mem_block_t));

    block->table = (char *)malloc(elem_num + 1);
    if (block->table == NULL) {
        fprintf(stderr, "malloc block->table error!!!!\n");
        return -1;
    }
    //初始化数组链表
    for (int i; i < elem_num; ++i) {
        block->table[i] = i + 1;
    }
    block->table[elem_num] = ENDOFMEM;

    block->data = (char *)malloc(elem_size * (elem_num + 1));
    if (block->data == NULL) {
        fprintf(stderr, "malloc block->data error!!!!\n");
        return -1;
    }
    memset(block->data, '\0', elem_size * (elem_num + 1));

    block->elem_num = elem_num;
    block->free_num = elem_num;

    block->head = 0;
    block->tail = elem_num;

    block->next = NULL;

    return block;
}

void *mem_pool_malloc(mem_pool_t *pool)
{
    mem_block_t *block = NULL, *pos = NULL;
    
    FOREACH_BLOCK_LIST (pos, pool->block_list) {
        if (pos->free_num > 0) {
            block = pos; 
            break;
        }
    }

    if (block == NULL) {
        // 创建新块
        block = __mem_pool_alloc_block(pool, pool->elem_size, pool->elem_num);
        if (block == NULL) {
            return NULL;
        }
    }

    int offset = block->head * block->elem_size;
    int head = block->table[block->head];
    block->table[block->head] = ALLOCATED;
    block->head = head;
    block->free_num -= 1;

    return block->data[offset];
}

void mem_pool_free(mem_pool_t *pool, void *p)
{
    int offset;
    int res;
    mem_block_t * block = NULL;
   
    FOREACH_BLOCK_LIST (block, pool->block_list) {
        offset = (block->data - p)/block->elem_size;
        // 属于这个块
        if (offset > 0 && offset < block->elem_num)  {
            memset(p, '\0', sizeof(block->elem_size)); 
            block->table[block->tail] = offset;
            block->table[offset] = ENDOFMEM; 
            block->free_num += 1;
            break;
        }
    }
}

void dump_mem_pool(mem_pool_t *pool)
{

}

struct test2{
    int a1;
    int a2;
    int a3;
    int b1;
    int b2;
    int b3;
    int c1;
};
struct test{
    int a;
    int b;
    int c;
};

int main()
{
    mem_pool_t *pool = mem_pool_create(sizeof(struct test));
    struct test *a = mem_pool_malloc(pool, sizeof(struct test));
    printf("pool block_num = %d, alloc_num = %d\n", pool->block_num, pool->alloc_num);
    a->a = 1; a->b = 2; a->c = 3;
    printf("free_num = %d\n", ((mem_node_t *)a-1)->block->free_num);
    struct test *a3 = mem_pool_malloc(pool, sizeof(struct test));
    printf("pool block_num = %d, alloc_num = %d\n", pool->block_num, pool->alloc_num);
    printf("free_num = %d\n", ((mem_node_t *)a-1)->block->free_num);
    mem_pool_free(pool, a3);
    printf("pool block_num = %d, alloc_num = %d\n", pool->block_num, pool->alloc_num);
    mem_pool_destroy(pool);
}
