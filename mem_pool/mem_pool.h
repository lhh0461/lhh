#ifndef	__MEM_POOL_H__

#define NOMAL         0
#define ALLOCATED    -1
#define ENDOFMEM     -2

typedef struct mem_block_s
{
    int free_num;
    int elem_num;
    int  *used_list;
    int  *free_list;
    void *data;
    mem_block_t *next_block;

}mem_block_t;

typedef struct mem_pool_s
{
    int elem_size;
    int elem_num;
    int total_alloc;
    mem_block_t *block_list;

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
