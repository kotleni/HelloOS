#pragma once

#include <kernel.h>
#include <misc/malloc.h>
#include <misc/string.h>
#include <fs/fat.h>

typedef struct {
    f32 *master_fs;
    //struct dir_entry file_ent;
    uint32_t curr_cluster;
    uint32_t file_size; // total file size
    uint32_t fptr; // index into the file
    uint32_t buffptr; // index into currbuf
    uint8_t currbuf[]; // flexible member for current cluster
} FILE;

FILE *fopen(const char *pathname, const char *mode);
int fclose(FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
//size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);