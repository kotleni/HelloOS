/* HelloOS */

#ifndef H_MALLOC
#define H_MALLOC

#include <types.h>
#include <misc/memory.h>
#include <kernel.h>

#define STATIC_ALLOC_SIZE 1024*1024*1024 // 1gb
#define MAX_MEM_NODES_COUNT 64

typedef struct {
    void* ptr;
    size_t size;
    bool is_free;
} mem_node;

void mm_init(uint32_t malloc_start_ptr);
void* malloc(size_t size);
void* realloc(void* oldptr, size_t size);
void free(void *fre);

#endif