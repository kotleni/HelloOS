/* HelloOS */

#pragma once

#include <types.h>
#include <misc/memory.h>
#include <kernel.h>

#define STATIC_ALLOC_SIZE 1024*1024*64 // 64mb

void mm_init(uint32_t kernel_end);
void* malloc(size_t size);
void free(void *fre);