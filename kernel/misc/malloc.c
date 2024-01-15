/* HelloOS */

#include <misc/malloc.h>

static byte heap[STATIC_ALLOC_SIZE] = {0}; 
static byte* free_ptr;

void mm_init(uint32_t kernel_end) {
	free_ptr = &heap;
	memset(heap, 0, sizeof(heap));
}

void* malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

	if (size > STATIC_ALLOC_SIZE - (free_ptr - heap)) {
		kern->panic("All available memory is allocated!");
        return NULL;
    }

    void* ptr = (void*)free_ptr;
    free_ptr += size;

	// kern->printf("Allocated ptr=%d, next ptr=%d, +%d bytes\n", ptr, free_ptr, (void*)free_ptr - ptr);

    return ptr;
}

void free(void *fre) {
	// TODO
}