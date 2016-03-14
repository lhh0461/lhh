#ifndef _FS_MALLOC_H_
#define _FS_MALLOC_H_

#ifdef __linux__
#include <stddef.h>
#define fs_malloc malloc
#define fs_calloc calloc
#define fs_realloc realloc
#define fs_free free

void * fs_malloc(size_t sz);
void * fs_calloc(size_t nmemb,size_t size);
void * fs_realloc(void *ptr, size_t size);
void fs_free(void *ptr);
#endif

#endif

