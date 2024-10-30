#include <stdio2.h>

static inline void parse_path(char *path, char **args, int *argc) {
    int len = strlen(path);

    char *buff = malloc(sizeof(char) * 32);
    for(int i = 0; i < len; i++) {
        char ch = path[i];
        
        //kprintf("buff is %s\n", buff);
        switch(ch) {
            case '/':
                if(strlen(buff) == 0) break;

                args[(*argc)++] = buff;
                buff = malloc(sizeof(char) * 32);
                break;
            default:
                char str[2];
                str[0] = ch;
                str[1] = '\0';
                strcat(buff, str);
                break;
        }
    }

    if(strlen(buff) > 0) {
        args[(*argc)++] = buff;
    }
}

static inline int entry_for_path(f32 *master_fs, char *path, dir_entry *entry) {
    char **path_args = malloc(sizeof(char*) * 8);
    int path_argc = 0;
    parse_path(path, path_args, &path_argc);

    directory dir;
    populate_root_dir(master_fs, &dir);
    int found_file = 0;
    if(path[0] != '/') {
        return found_file;
    }

    dir_entry *currentry = NULL;
    entry->name = NULL;

    for(int i = 0; i < path_argc; i++) {
        char* name = path_args[i];

        int found = 0;
        for(int entryi = 0; entryi < dir.num_entries; entryi++) {
            currentry = &dir.entries[entryi];
            //kprintf("strcmp('%s', '%s')\n", currentry->name, name);
            if(strcmp(currentry->name, name) == 0) {
                if(entry->name) free(entry->name);
                *entry = *currentry;
                // TODO: Make sure this doesn't leak. Very dangerous:
                entry->name = strdup(currentry->name);

                uint32_t cluster = currentry->first_cluster;
                free_directory(master_fs, &dir);
                populate_dir(master_fs, &dir, cluster);
                found = 1;
                break;

            }
        }
        if(!found) {
            free_directory(master_fs, &dir);
            return 0;
        }
    }

    free_directory(master_fs, &dir);
    return 1;
}

FILE *fopen(char *pathname, char *mode) {
    f32 *master_fs = fat_open(pathname);

    dir_entry entry;
    if(!entry_for_path(master_fs, pathname, &entry)) {
        free(entry.name);
        return NULL;
    }
    //kprintf("Got entry: %s [%d]\n", entry.name, entry.first_cluster);
    free(entry.name);

    FILE *f = malloc(sizeof (FILE) + master_fs->cluster_size);
    f->curr_cluster = entry.first_cluster;
    f->file_size = entry.file_size;
    f->fptr = 0;
    f->buffptr = 0;
    f->master_fs = master_fs;
    getCluster(master_fs, f->currbuf, f->curr_cluster);
    return f;
}

int fclose(FILE *stream) {
    free(stream);
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t bytes_to_read = size * nmemb;
    size_t bytes_read = 0;

    if(stream->fptr + bytes_to_read > stream->file_size) {
        bytes_to_read = stream->file_size - stream->fptr;
    }
    //printf("Reading %d bytes.\n", bytes_to_read);
    while(bytes_to_read > 0) {
        if(stream->buffptr + bytes_to_read > stream->master_fs->cluster_size) {
            // Need to read at least 1 more cluster
            size_t to_read_in_this_cluster = stream->master_fs->cluster_size - stream->buffptr;
            memcpy(ptr + bytes_read, stream->currbuf + stream->buffptr, to_read_in_this_cluster);
            bytes_read += to_read_in_this_cluster;
            //printf("buffptr = 0\n");
            stream->buffptr = 0;
            //printf("Getting next cluster.\n");
            stream->curr_cluster = get_next_cluster_id(stream->master_fs, stream->curr_cluster);
            //printf("Next cluster: %x\n", stream->curr_cluster);
            if(stream->curr_cluster >= EOC) {
                //printf("Returning.\n");
                stream->fptr += bytes_read;
                return bytes_read;
            }
            //printf("getting next cluster.\n");
            getCluster(stream->master_fs, stream->currbuf, stream->curr_cluster);
            bytes_to_read -= to_read_in_this_cluster;
        }
        else {
            //printf("buffptr: %d\n", stream->buffptr);
            memcpy(ptr + bytes_read, stream->currbuf + stream->buffptr, bytes_to_read);
            bytes_read += bytes_to_read;
            stream->buffptr += bytes_to_read;
            bytes_to_read = 0;
        }
    }
    //printf("Returning.\n");
    stream->fptr += bytes_read;
    return bytes_read;
}