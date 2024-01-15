/* HelloOS */

#include <misc/malloc.h>

//static byte heap[STATIC_ALLOC_SIZE] = {0}; 
static byte *start_ptr;
static mem_node nodes[MAX_MEM_NODES_COUNT] = {0};
static byte* free_ptr;
static uint32_t next_node_index;

void mm_init(uint32_t malloc_start_ptr) {
    start_ptr = (byte*) malloc_start_ptr;
	free_ptr = (byte*)start_ptr;
	//memset(start_ptr, 0, STATIC_ALLOC_SIZE);
}

void* malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

	if (size > STATIC_ALLOC_SIZE - (free_ptr - start_ptr)) {
		kpanic("All available memory is allocated!");
        return NULL;
    }

    // First, try to find free-ed node with good size
    for(uint32_t i; i < next_node_index; i++) {
        if(nodes[i].size <= size && nodes[i].is_free) {
            return nodes[i].ptr;
        }
    }

    // If not found, create new node
    if(next_node_index >= MAX_MEM_NODES_COUNT-1) {
        kpanic("All nodes are used!");
    }

    void* ptr = (void*)free_ptr;
    free_ptr += size;

    mem_node node;
    node.ptr = ptr;
    node.size = size;
    node.is_free = false;

    nodes[next_node_index] = node;
    next_node_index++;

	//kprintf("Allocated ptr=%d, next ptr=%d, +%d bytes\n", ptr, free_ptr, (void*)free_ptr - ptr);

    return ptr;
}

void* realloc(void* oldptr, size_t size) {
    free(oldptr);
    return malloc(size);
}

void free(void *fre) {
	for(uint32_t i; i < next_node_index; i++) {
        if(nodes[i].ptr == fre) {
            nodes[i].is_free = true;
        }
    }
}