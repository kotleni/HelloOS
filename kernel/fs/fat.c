#include <fs/fat.h>

void next_dir_entry(f32 *fs, uint8_t *root_cluster, uint8_t *entry, uint8_t **nextentry, dir_entry *target_dirent, uint32_t cluster, uint32_t *secondcluster);

static void trim_spaces(char *c, int max) {
    int i = 0;
    while(*c != ' ' && i++ < max) {
        c++;
    }
    if(*c == ' ') *c = 0;
}

static void getSector(f32 *fs, uint8_t *buff, uint32_t sector, uint32_t count) {
    ata_pio_read48(sector, count, buff);
}

static uint16_t readi16(uint8_t *buff, size_t offset)
{
    uint8_t *ubuff = buff + offset;
    return ubuff[1] << 8 | ubuff[0];
}

static inline uint32_t readi32(uint8_t *buff, size_t offset) {
    uint8_t *ubuff = buff + offset;
    return
        ((ubuff[3] << 24) & 0xFF000000) |
        ((ubuff[2] << 16) & 0x00FF0000) |
        ((ubuff[1] << 8) & 0x0000FF00) |
        (ubuff[0] & 0x000000FF);
}

static uint8_t checksum_fname(char *fname) {
    uint32_t i;
    uint8_t checksum = 0;
    for(i = 0; i < 11; i++) {
        uint8_t highbit = (checksum & 0x1) << 7;
        checksum = ((checksum >> 1) & 0x7F) | highbit;
        checksum = checksum + fname[i];
    }
    return checksum;
}

static void read_bpb(f32 *fs) {
    uint8_t sector0[512];
    ata_pio_read48(0, 1, sector0);

    fs->bpb.bytes_per_sector = readi16(sector0, 11);
    fs->bpb.sectors_per_cluster = sector0[13];
    fs->bpb.reserved_sectors = readi16(sector0, 14);
    fs->bpb.FAT_count = sector0[16];
    fs->bpb.dir_entries = readi16(sector0, 17);
    fs->bpb.total_sectors = readi16(sector0, 19);
    fs->bpb.media_descriptor_type = sector0[21];
    fs->bpb.count_sectors_per_FAT12_16 = readi16(sector0, 22);
    fs->bpb.count_sectors_per_track = readi16(sector0, 24);
    fs->bpb.count_heads_or_sizes_on_media = readi16(sector0, 26);
    fs->bpb.count_hidden_sectors = readi32(sector0, 28);
    fs->bpb.large_sectors_on_media = readi32(sector0, 32);
    // EBR
    fs->bpb.count_sectors_per_FAT32 = readi32(sector0, 36);
    fs->bpb.flags = readi16(sector0, 40);
    fs->bpb.FAT_version = readi16(sector0, 42);
    fs->bpb.cluster_number_root_dir = readi32(sector0, 44);
    fs->bpb.sector_number_FSInfo = readi16(sector0, 48);
    fs->bpb.sector_number_backup_boot_sector = readi16(sector0, 50);
    // Skip 12 bytes
    fs->bpb.drive_number = sector0[64];
    fs->bpb.windows_flags = sector0[65];
    fs->bpb.signature = sector0[66];
    fs->bpb.volume_id = readi32(sector0, 67);
    memcpy(&fs->bpb.volume_label, sector0 + 71, 11); fs->bpb.volume_label[11] = 0;
    memcpy(&fs->bpb.system_id, sector0 + 82, 8); fs->bpb.system_id[8] = 0;
}

static uint32_t sector_for_cluster(f32 *fs, uint32_t cluster) {
    return fs->cluster_begin_sector + ((cluster - 2) * fs->bpb.sectors_per_cluster);
}

// CLUSTER NUMBERS START AT 2 (for some reason...)
void getCluster(f32 *fs, uint8_t *buff, uint32_t cluster_number) { // static
    if(cluster_number >= EOC) {
        kpanic("Can't get cluster. Hit End Of Chain.");
    }
    uint32_t sector = sector_for_cluster(fs, cluster_number);
    uint32_t sector_count = fs->bpb.sectors_per_cluster;
    getSector(fs, buff, sector, sector_count);
}

uint32_t get_next_cluster_id(f32 *fs, uint32_t cluster) { // static
    return fs->FAT[cluster] & 0x0FFFFFFF;
}

static char *parse_long_name(uint8_t *entries, uint8_t entry_count) {
    // each entry can hold 13 characters.
    char *name = malloc(entry_count * 13);
    int i, j;
    for(i = 0; i < entry_count; i++) {
        uint8_t *entry = entries + (i * 32);
        uint8_t entry_no = (uint8_t)entry[0] & 0x0F;
        char *name_offset = name + ((entry_no - 1) * 13);

        for(j = 1; j < 10; j+=2) {
            if(entry[j] >= 32 && entry[j] <= 127) {
                *name_offset = entry[j];
            }
            else {
                *name_offset = 0;
            }
            name_offset++;
        }
        for(j = 14; j < 25; j+=2) {
            if(entry[j] >= 32 && entry[j] <= 127) {
                *name_offset = entry[j];
            }
            else {
                *name_offset = 0;
            }
            name_offset++;
        }
        for(j = 28; j < 31; j+=2) {
            if(entry[j] >= 32 && entry[j] <= 127) {
                *name_offset = entry[j];
            }
            else {
                *name_offset = 0;
            }
            name_offset++;
        }
    }
    return name;
}

// static void clear_cluster(f32 *fs, uint32_t cluster) {
//     uint8_t buffer[fs->cluster_size];
//     memset(buffer, 0, fs->cluster_size);
//     putCluster(fs, buffer, cluster);
// }

// static uint32_t allocateCluster(f32 *fs) {
//     uint32_t i, ints_per_fat = (512 * fs->bpb.count_sectors_per_FAT32) / 4;
//     for(i = fs->cluster_alloc_hint; i < ints_per_fat; i++) {
//         if(fs->FAT[i] == 0) {
//             fs->FAT[i] = 0x0FFFFFFF;
//             clear_cluster(fs, i);
//             fs->cluster_alloc_hint = i+1;
//             return i;
//         }
//     }
//     for(i = 0; i < fs->cluster_alloc_hint; i++) {
//         if(fs->FAT[i] == 0) {
//             fs->FAT[i] = 0x0FFFFFFF;
//             clear_cluster(fs, i);
//             fs->cluster_alloc_hint = i+1;
//             return i;
//         }
//     }
//     return 0;
// }

static uint8_t *locate_entries(f32 *fs, uint8_t *cluster_buffer, directory *dir, uint32_t count, uint32_t *found_cluster) {
    uint32_t dirs_per_cluster = fs->cluster_size / 32;

    uint32_t i;
    int64_t index = -1;
    uint32_t cluster = dir->cluster;
    while(1) {
        getCluster(fs, cluster_buffer, cluster);

        uint32_t in_a_row = 0;
        for(i = 0; i < dirs_per_cluster; i++) {
            uint8_t *entry = cluster_buffer + (i * 32);
            uint8_t first_byte = entry[0];
            if(first_byte == 0x00 || first_byte == 0xE5) {
                in_a_row++;
            }
            else {
                in_a_row = 0;
            }

            if(in_a_row == count) {
                index = i - (in_a_row - 1);
                break;
            }
        }
        if(index >= 0) {
            // We found a spot to put our crap!
            break;
        }

        uint32_t next_cluster = fs->FAT[cluster];
        if(next_cluster >= EOC) {
            // next_cluster = allocateCluster(fs);
            // if(!next_cluster) {
            //     return 0;
            // }
            // fs->FAT[cluster] = next_cluster;
            kpanic("WTF? allocateCluster not should be executed in readonly FS!");
        }
        cluster = next_cluster;
    }
    *found_cluster = cluster;
    return cluster_buffer + (index * 32);
}

const bios_parameter_block *getBPB(f32 *fs) {
    return &fs->bpb;
}

void populate_dir(f32 *fs, directory *dir, uint32_t cluster) {
    dir->cluster = cluster;
    uint32_t dirs_per_cluster = fs->cluster_size / 32;
    uint32_t max_dirs = 0;
    dir->entries = 0;
    uint32_t entry_count = 0;

    while(1) {
        max_dirs += dirs_per_cluster;
        dir->entries = realloc(dir->entries, max_dirs * sizeof (dir_entry));
        // Double the size in case we need to read a directory entry that
        // spans the bounds of a cluster.
        uint8_t root_cluster[fs->cluster_size * 2];
        getCluster(fs, root_cluster, cluster);

        uint8_t *entry = root_cluster;
        while((uint32_t)(entry - root_cluster) < fs->cluster_size) {
            uint8_t first_byte = *entry;
            if(first_byte == 0x00 || first_byte == 0xE5) {
                // This directory entry has never been written
                // or it has been deleted.
                entry += 32;
                continue;
            }

            uint32_t secondcluster = 0;
            uint8_t *nextentry = NULL;
            dir_entry *target_dirent = dir->entries + entry_count;
            next_dir_entry(fs, root_cluster, entry, &nextentry, target_dirent, cluster, &secondcluster);
            entry = nextentry;
            if(secondcluster) {
                cluster = secondcluster;
            }

            entry_count++;
        }
        cluster = get_next_cluster_id(fs, cluster);
        if(cluster >= EOC) break;
    }
    dir->num_entries = entry_count;
}

void populate_root_dir(f32 *fs, directory *dir) {
    populate_dir(fs, dir, 2);
}

// Populates dirent with the directory entry starting at start
// Returns a pointer to the next 32-byte chunk after the entry
// or NULL if either start does not point to a valid entry, or
// there are not enough entries to build a dir_entry
static uint8_t *read_dir_entry(f32 *fs, uint8_t *start, uint8_t *end, dir_entry *dirent) {
    uint8_t first_byte = start[0];
    uint8_t *entry = start;
    if(first_byte == 0x00 || first_byte == 0xE5) {
        // NOT A VALID ENTRY!
        return NULL;
    }

    uint32_t LFNCount = 0;
    while(entry[11] == LFN) {
        LFNCount++;
        entry += 32;
        if(entry == end) {
            return NULL;
        }
    }
    if(LFNCount > 0) {
        dirent->name = parse_long_name(start, LFNCount);
    }
    else {
        // There's no long file name.
        // Trim up the short filename.
        dirent->name = malloc(13);
        memcpy(dirent->name, entry, 11);
        dirent->name[11] = 0;
        char extension[4];
        memcpy(extension, dirent->name + 8, 3);
        extension[3] = 0;
        trim_spaces(extension, 3);

        dirent->name[8] = 0;
        trim_spaces(dirent->name, 8);

        if(strlen(extension) > 0) {
            uint32_t len = strlen(dirent->name);
            dirent->name[len++] = '.';
            memcpy(dirent->name + len, extension, 4);
        }
    }

    dirent->dir_attrs = entry[11];;
    uint16_t first_cluster_high = readi16(entry, 20);
    uint16_t first_cluster_low = readi16(entry, 26);
    dirent->first_cluster = first_cluster_high << 16 | first_cluster_low;
    dirent->file_size = readi32(entry, 28);
    return entry + 32;
}

// This is a complicated one. It parses a directory entry into the dir_entry pointed to by target_dirent.
// root_cluster  must point to a buffer big enough for two clusters.
// entry         points to the entry the caller wants to parse, and must point to a spot within root_cluster.
// nextentry     will be modified to hold the next spot within root_entry to begin looking for entries.
// cluster       is the cluster number of the cluster loaded into root_cluster.
// secondcluster will be modified IF this function needs to load another cluster to continue parsing
//               the entry, in which case, it will be set to the value of the cluster loaded.
//
void next_dir_entry(f32 *fs, uint8_t *root_cluster, uint8_t *entry, uint8_t **nextentry, dir_entry *target_dirent, uint32_t cluster, uint32_t *secondcluster) {

    uint8_t *end_of_cluster = root_cluster + fs->cluster_size;
    *nextentry = read_dir_entry(fs, entry, end_of_cluster, target_dirent);
    if(!*nextentry) {
        // Something went wrong!
        // Either the directory entry spans the bounds of a cluster,
        // or the directory entry is invalid.
        // Load the next cluster and retry.

        // Figure out how much of the last cluster to "replay"
        uint32_t bytes_from_prev_chunk = end_of_cluster - entry;

        *secondcluster = get_next_cluster_id(fs, cluster);
        if(*secondcluster >= EOC) {
            // There's not another directory cluster to load
            // and the previous entry was invalid!
            // It's possible the filesystem is corrupt or... you know...
            // my software could have bugs.
            kprintf("FOUND BAD DIRECTORY ENTRY!");
        }
        // Load the cluster after the previous saved entries.
        getCluster(fs, root_cluster + fs->cluster_size, *secondcluster);
        // Set entry to its new location at the beginning of root_cluster.
        entry = root_cluster + fs->cluster_size - bytes_from_prev_chunk;

        // Retry reading the entry.
        *nextentry = read_dir_entry(fs, entry, end_of_cluster + fs->cluster_size, target_dirent);
        if(!*nextentry) {
            // Still can't parse the directory entry.
            // Something is very wrong.
            kpanic("FAILED TO READ DIRECTORY ENTRY! THE SOFTWARE IS BUGGY!\n");
        }
    }
}

// static void zero_FAT_chain(f32 *fs, uint32_t start_cluster) {
//     uint32_t cluster = start_cluster;
//     while(cluster < EOC && cluster != 0) {
//         uint32_t next_cluster = fs->FAT[cluster];
//         fs->FAT[cluster] = 0;
//         cluster = next_cluster;
//     }
//     flushFAT(fs);
// }

void free_directory(f32 *fs, directory *dir){
    uint32_t i;
    for(i = 0; i < dir->num_entries; i++) {
        free(dir->entries[i].name);
    }
    free(dir->entries);
}

uint8_t *readFile(f32 *fs, dir_entry *dirent) {
    uint8_t *file = malloc(dirent->file_size);
    uint8_t *filecurrptr = file;
    uint32_t cluster = dirent->first_cluster;
    uint32_t copiedbytes = 0;
    while(1) {
        uint8_t cbytes[fs->cluster_size];
        getCluster(fs, cbytes, cluster);

        uint32_t remaining = dirent->file_size - copiedbytes;
        uint32_t to_copy = remaining > fs->cluster_size ? fs->cluster_size : remaining;

        memcpy(filecurrptr, cbytes, to_copy);

        filecurrptr += fs->cluster_size;
        copiedbytes += to_copy;

        cluster = get_next_cluster_id(fs, cluster);
        if(cluster >= EOC) break;
    }
    return file;
}

void print_directory(f32 *fs, directory *dir) {
    uint32_t i;
    uint32_t max_name = 0;
    for(i = 0; i < dir->num_entries; i++) {
        uint32_t namelen = strlen(dir->entries[i].name);
        max_name = namelen > max_name ? namelen : max_name;
    }

    char *namebuff = malloc(max_name + 1);
    for(i = 0; i < dir->num_entries; i++) {
//        kprintf("[%d] %*s %c %8d bytes ",
//               i,
//               -max_name,
//               dir->entries[i].name,
//               dir->entries[i].dir_attrs & DIRECTORY?'D':' ',
//               dir->entries[i].file_size, dir->entries[i].first_cluster);
        kprintf("[%d] ", i);


        uint32_t j;
        for(j = 0; j < max_name; j++) {
            namebuff[j] = ' ';
        }
        namebuff[max_name] = 0;
        for(j = 0; j < strlen(dir->entries[i].name); j++) {
            namebuff[j] = dir->entries[i].name[j];
        }

        kprintf("%s %c %d ",
               namebuff,
               dir->entries[i].dir_attrs & DIRECTORY?'D':' ',
               dir->entries[i].file_size);

        uint32_t cluster = dir->entries[i].first_cluster;
        uint32_t cluster_count = 1;
        while(1) {
            cluster = fs->FAT[cluster];
            if(cluster >= EOC) break;
            if(cluster == 0) {
                kpanic("BAD CLUSTER CHAIN! FS IS CORRUPT!");
            }
            cluster_count++;
        }
        kprintf("clusters: [%d]\n", cluster_count);
    }
    free(namebuff);
}

uint32_t count_free_clusters(f32 *fs) {
    uint32_t clusters_in_fat = (fs->bpb.count_sectors_per_FAT32 * 512) / 4;
    uint32_t i;
    uint32_t count = 0;
    for(i = 0; i < clusters_in_fat; i++) {
        if(fs->FAT[i] == 0) {
            count++;
        }
    }
    return count;
}

bool fat_init() {
    
}

f32 *fat_open(char *path) {
    f32 *fs = malloc(sizeof(f32));
    //kprintf("master_fs ptr is %d\n", fs);
    if(ata_identify() == 0) {
        return NULL;
    }
    kprintf("ATA is identified!\n");
    read_bpb(fs);

    // Check is FAT32
    trim_spaces(fs->bpb.system_id, 8);
    if(strcmp(fs->bpb.system_id, "FAT32") != 0) {
        kprintf("FS(%s) is not FAT32!\n", fs->bpb.system_id);
        free(fs);
        return NULL;
    }

    kprintf("Sectors per cluster: %d\n", fs->bpb.sectors_per_cluster);

    fs->partition_begin_sector = 0;
    fs->fat_begin_sector = fs->partition_begin_sector + fs->bpb.reserved_sectors;
    fs->cluster_begin_sector = fs->fat_begin_sector + (fs->bpb.FAT_count * fs->bpb.count_sectors_per_FAT32);
    fs->cluster_size = 512 * fs->bpb.sectors_per_cluster;
    fs->cluster_alloc_hint = 0;

    // Load the FAT
    uint32_t bytes_per_fat = 512 * fs->bpb.count_sectors_per_FAT32;
    fs->FAT = malloc(bytes_per_fat);
    uint32_t sector_i;
    for(sector_i = 0; sector_i < fs->bpb.count_sectors_per_FAT32; sector_i++) {
        uint8_t sector[512];
        getSector(fs, sector, fs->fat_begin_sector + sector_i, 1);
        uint32_t integer_j;
        for(integer_j = 0; integer_j < 512/4; integer_j++) {
            fs->FAT[sector_i * (512 / 4) + integer_j]
                = readi32(sector, integer_j * 4);
        }
    }

    return fs;
}

void fat_close(f32* fs) {
    kprintf("Destroying FAT32 filesystem.\n");
    free(fs->FAT);
    free(fs);
}