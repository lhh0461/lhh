#ifndef	__MEM_POOL_H__
#define	__MEM_POOL_H__

#define ALLOCATED    -1
#define ENDOFMEM     -2


//TODO 数组实现链表
typedef struct mem_block_s
{
    int elem_num; //元素总个数
    int elem_size;//每个元素大小
    int head; //链表头
    int tail; //链表尾 
    int *table;//空闲链表
    char *data;//内存池分配的数据
    struct mem_block_s *next;//内存块链表

}mem_block_t;

typedef struct mem_pool_s
{
    int elem_size;//每个元素大小
    int elem_num;//每次创建元素个数
    int elem_total;//元素总个数
    mem_block_t *head;//内存块链表头
    mem_block_t *tail;//内存块链表尾

}mem_pool_t;



mem_pool_t *mem_pool_create(int elem_size, int elem_num);
int mem_pool_destroy(mem_pool_t *pool);
static inline mem_block_t *__mem_pool_alloc_block(mem_pool_t *pool, int elem_size, int elem_num);
static inline void __mem_pool_free_block(mem_block_t *block);
void *mem_pool_malloc(mem_pool_t *pool);
void mem_pool_free(mem_pool_t *pool, void *p);
void dump_mem_pool(mem_pool_t *pool);


#endif	/* __MEM_POOL_H__ */
