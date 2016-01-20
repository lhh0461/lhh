#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem_pool.h"

#define BLOCK_NODE_NUM 10

mem_pool_t * mem_pool_create(int block_size)
{
    mem_pool_t *pool = NULL;
    pool = (mem_pool_t *)malloc(sizeof(mem_pool_t));
    if (pool == NULL) {
        fprintf(stderr, "malloc pool error!!!!\n");
        return NULL;
    }
    memset(pool, 0, sizeof(mem_pool_t));
    INIT_LIST_HEAD(&pool->block_list);
    pool->block_num = 0;
    pool->alloc_num = 0;

    mem_block_t *blk = mem_pool_alloc_block(pool, block_size);
    list_add_tail(&blk->list, &pool->block_list);
    return pool;
}

int mem_pool_destroy(mem_pool_t *pool)
{
   //TODO 

}

mem_block_t *mem_pool_alloc_block(mem_pool_t *pool, int block_size)
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
    block->size = block_size;
    block->free_num = 0;
    block->used_num = 0;
    INIT_LIST_HEAD(&block->free_list);
    INIT_LIST_HEAD(&block->used_list);
    mem_pool_block_add_node(block, BLOCK_NODE_NUM);

    pool->block_num++;

    return block;
}

static int mem_pool_block_add_node(mem_block_t *block, int node_num)
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

void *mem_pool_malloc(mem_pool_t *pool, int size)
{
    mem_block_t *block = NULL, *tmp = NULL;
    struct list_head *pos;

    list_for_each (pos, &pool->block_list) {
        tmp = list_entry(pos, struct mem_block_s, list); 
        if (tmp->size >= size) {
            if (block == NULL || tmp->size < block->size) {
                block = tmp;
            }
        }
    }

    if (block == NULL) {
        //TODO：需要考虑对齐
        block = mem_pool_alloc_block(pool, size);
        if (block == NULL) return NULL;
    }
    
    if (block->free_num == 0) {
        mem_pool_block_add_node(block, BLOCK_NODE_NUM);
    }

    mem_node_t *node = NULL;
    list_for_each (pos, &block->free_list) {
        node = list_entry(pos, struct mem_node_s, list); 
        break;
    }

    list_del(&node->list);
    list_add_tail(&node->list, &block->used_list);
    block->free_num--;
    block->used_num++;
    pool->alloc_num += size;

    return node->data;
}

void mem_pool_free(mem_pool_t *pool, void *p)
{
    mem_node_t *node = (mem_node_t *)p - 1;
    
    list_del(&node->list);
    list_add_tail(&node->list, &node->block->free_list);
    node->block->free_num--;
    node->block->used_num++;
    pool->alloc_num -= node->block->size;
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
}
