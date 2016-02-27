#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem_pool.h"

#define FOREACH_BLOCK_LIST(pos, head) \
    for (pos = head; pos != NULL; pos = pos->next)

mem_pool_t * mem_pool_create(int elem_size, int elem_num)
{
    mem_pool_t *pool = NULL;
    pool = (mem_pool_t *)malloc(sizeof(mem_pool_t));
    if (pool == NULL) {
        fprintf(stderr, "malloc pool error!!!!\n");
        return NULL;
    }
    memset(pool, '\0', sizeof(mem_pool_t));
    pool->elem_size = elem_size;
    pool->elem_num = elem_num;
    pool->elem_total = elem_num;

    mem_block_t *blk = __mem_pool_alloc_block(pool, elem_size, elem_num); 
    pool->head = blk;
    pool->tail = blk;
    
    fprintf(stdout, "create mem_pool success.\n");
    return pool;
}

int mem_pool_destroy(mem_pool_t *pool)
{
//    if (pool == NULL) {
//        fprintf(stderr, "pool is NULL!!!!\n");
//        return -1;
//    }
//     
//    mem_block_t *block = NULL;
//    mem_node_t *node = NULL;
//    struct list_head *pos, *used_pos, *free_pos;
//
//    list_for_each (pos, &pool->block_list) {
//        block = list_entry(pos, struct mem_block_s, list); 
//        
//        list_for_each (used_pos, &block->used_list) {
//            node = list_entry(used_pos, struct mem_node_s, list); 
//            list_del(&node->list);
//            free(node);
//        }
//        
//        list_for_each (free_pos, &block->free_list) {
//            node = list_entry(free_pos, struct mem_node_s, list); 
//            list_del(&node->list);
//            free(node);
//        }
//
//        list_del(&block->list);
//        free(block);
//    }
//    free(pool);
//    fprintf(stdout, "destory mem_pool success.\n");
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

    block->table = (int *)malloc(elem_num + 1);
    if (block->table == NULL) {
        fprintf(stderr, "malloc block->table error!!!!\n");
        return NULL;
    }
    int i;
    //初始化数组链表
    for (i = 0; i < elem_num; ++i) {
        block->table[i] = i + 1;
    }
    block->table[elem_num] = ENDOFMEM;

    block->data = (char *)malloc(elem_size * (elem_num + 1));
    if (block->data == NULL) {
        fprintf(stderr, "malloc block->data error!!!!\n");
        return NULL;
    }
    memset(block->data, '\0', elem_size * (elem_num + 1));

    block->elem_num = elem_num;
    block->free_num = elem_num;
    block->elem_size = elem_size;

    block->head = 0;
    block->tail = elem_num;

    block->next = NULL;

    fprintf(stdout, "create mem block success.\n");
    return block;
}

void *mem_pool_malloc(mem_pool_t *pool)
{
    mem_block_t *block = NULL, *pos = NULL;
    
    //找出还没满的内存块
    FOREACH_BLOCK_LIST (pos, pool->head) {
        if (pos->head != pos->tail) {
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
        pool->tail->next = block;
        pool->tail = block;
        pool->elem_total += pool->elem_num;
        
    }

    int offset = block->head * block->elem_size;
    int head = block->table[block->head];
    block->table[block->head] = ALLOCATED;
    block->head = head;

    return block->data + offset;
}

void mem_pool_free(mem_pool_t *pool, void *p)
{
    int offset;
    int res;
    mem_block_t * block = NULL;
   
    FOREACH_BLOCK_LIST (block, pool->head) {
        offset = (block->data - (char *)p)/block->elem_size;
        // 属于这个块
        if (offset >= 0 && offset < block->elem_num)  {
            memset(p, '\0', sizeof(block->elem_size)); 
            block->table[block->tail] = offset;
            block->table[offset] = ENDOFMEM; 
            break;
        }
    }
}

void dump_mem_pool(mem_pool_t *pool)
{
    mem_block_t * block = NULL;

    printf("pool->elem_size = %d, pool->elem_total = %d\n", pool->elem_size, pool->elem_total);
    FOREACH_BLOCK_LIST (block, pool->head) {
        printf("block->elem_num = %d\n", block->elem_num);
        printf("block->elem_size = %d\n", block->elem_size);
        printf("block->head = %d\n", block->head);
        printf("block->tail = %d\n", block->tail);
    }
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
    char b;
    int c;
};

int main()
{
   printf("start \n");
   mem_pool_t *pool = mem_pool_create(sizeof(struct test), 10);
   printf("start 2\n");
   int i = 0;
   for (; i < 20; i++) {
       struct test *a = mem_pool_malloc(pool);
       //printf("i = %d, pool elem_num = %d, elem_size = %d\n", i, pool->elem_num, pool->elem_size);
       a->a = 1;
       a->b = 2;
       a->c = 3;
       //printf("a->a = %d, a->b = %d, a->c = %d\n", a->a, a->b, a->c);
       //printf("a->a = %d, a->b = %d, a->c = %d\n", a->a, a->b, a->c);
       dump_mem_pool(pool);
   }
   return 0;
}
