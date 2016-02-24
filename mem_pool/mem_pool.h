#ifndef	__MEM_POOL_H__

#define ALLOCATED    -1
#define ENDOFMEM     -2


//TODO 数组实现链表
typedef struct mem_block_s
{
    int free_num; //空闲节点数
    int elem_num; //元素总个数
    int elem_size;//每个元素大小
    int head; //链表头
    int tail; //链表尾 
    int *table;//空闲链表
    void *data;//内存池分配的数据
    mem_block_t *next;

}mem_block_t;

typedef struct mem_pool_s
{
    int elem_size;
    int elem_num;
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
