#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem_pool.h"

#define BLOCK_NODE_NUM 10

mem_pool_t * mem_pool_create(int elem_size, int elem_num)
{
    mem_pool_t *pool = NULL;
    pool = (mem_pool_t *)malloc(sizeof(mem_pool_t));
    if (pool == NULL) {
        fprintf(stderr, "malloc pool error!!!!\n");
        return NULL;
    }
    memset(pool, 0, sizeof(mem_pool_t));
    pool->alloc_num = 0;
    pool->elem_size = 0;
    pool->elem_num = 0;

    mem_block_t *blk = mem_pool_alloc_block(pool, elem_size, elem_num); 
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

static inline mem_block_t *mem_pool_alloc_block(mem_pool_t *pool, int elem_size, int elem_num)
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
    memset(block->table, NOMAL, elem_num + 1);

    block->data = (char *)malloc(elem_size * (elem_num + 1));
    if (block->data == NULL) {
        fprintf(stderr, "malloc block->data error!!!!\n");
        return -1;
    }
    memset(block->data, '\0', elem_size * (elem_num + 1));

    block->elem_num = elem_num;
    block->free_num = elem_num;
    block->next_block = NULL;

    return block;
}

static int __mem_pool_block_add_node(mem_block_t *block, int elem_size, int elem_num)
{
    mem_node_t *node = NULL;
    int i;
    for (i = 0; i < node_num; ++i) {
        node = (mem_node_t *)malloc(sizeof(struct mem_node_s) + block->size);
        if (node == NULL) {
            fprintf(stderr, "malloc node error!!!!\n");
            return -1;
        }
        memset(node, 0, sizeof(mem_node_t));
        node->block = block;
        //node->data = (char *)(node + 1);
        list_add_tail(&node->list, &block->free_list);
        block->free_num++;
        node = NULL;
    }
    return 0;
}

void *mem_pool_malloc(mem_pool_t *pool)
{
    mem_block_t *block = NULL, *tmp = NULL;
    struct list_head *pos;
    
    for (tmp = pool->block_list; tmp != NULL; tmp = tmp->next_block) {
        if (tmp->free_num > 0) {
            block = tmp; 
            break;
        }
    }

    if (block == NULL) {
        // 创建新块
        block = mem_pool_alloc_block(pool, pool->elem_size, pool->elem_num);
        if (block == NULL) {
            return NULL;
        }
    }

    int i = 0;
    for (; i < block->elem_num; ++i) {
        if (block->table[i] == NOMAL) {
            break;
        }
    }
    assert(i < block->elem_num);
    block->table[i] = ALLOCATED;
    block->free_num -= 1;

    return block->data[pool->elem_size * i];
}

void mem_pool_free(mem_pool_t *pool, void *p)
{
    
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
