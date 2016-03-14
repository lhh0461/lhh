#ifdef __linux__
#include "fs_malloc.h"
#include <jemalloc.h>
#include <stdio.h>

void * fs_malloc(size_t sz)
{
    fprintf(stderr, "in fs malloc\n");
    return je_malloc(sz);
}
void * fs_calloc(size_t nmemb, size_t size)
{
    return je_calloc(nmemb, size);
}
void * fs_realloc(void *ptr, size_t size)
{
    return je_realloc(ptr, size);
}
void fs_free(void *ptr)
{
    je_free(ptr);
}

#endif
