#include <fs/fat.h>

void kdumpmem(uint8_t *ptr, int size) {
    int bytesPerBlock = 4;
    int blocksPerLine = 4;
    int bytesInLine = bytesPerBlock * blocksPerLine;
    int lines = size / bytesInLine;
    int remainingBytes = size % bytesInLine;

    int index = 0;
    for (int l = 0; l < lines; l++) {
        for (int bl = 0; bl < blocksPerLine; bl++) {
            for (int b = 0; b < bytesPerBlock; b++) {
                uint8_t bbyte = ptr[index];
                char hex[3];
                byte_to_hex(bbyte, hex);
                printf("%s", hex);

                index++;
            }
            printf(" ");
        }

        // ASCII block
        printf(" | ");
        for (int i = index - bytesInLine; i < index; i++) {
            char c = (ptr[i] >= 32 && ptr[i] <= 126) ? ptr[i] : '.';
            printf("%c", c); 
        }

        printf("\n");
    }

    // Process remaining bytes if any
    if (remainingBytes > 0) {
        // Hex dump for remaining bytes
        for (int i = 0; i < remainingBytes; i++) {
            uint8_t bbyte = ptr[index];
            char hex[3];
            byte_to_hex(bbyte, hex);
            printf("%s", hex);
            index++;

            if ((i + 1) % bytesPerBlock == 0) {
                printf(" ");
            }
        }

        // Add spaces to align ASCII block if last line is shorter
        int spaces = (bytesInLine - remainingBytes) * 2 + (blocksPerLine - (remainingBytes / bytesPerBlock));
        for (int i = 0; i < spaces; i++) {
            printf(" ");
        }

        // ASCII block for remaining bytes
        printf(" | ");
        for (int i = size - remainingBytes; i < size; i++) {
            char c = (ptr[i] >= 32 && ptr[i] <= 126) ? ptr[i] : '.';
            printf("%c", c);
        }
        printf("\n");
    }
}


#define ENTRY_SIZE 32
bool fat_init() {
    // Allocate memory for BPB structure and read it from disk
    bpb_t *bpb = (bpb_t*) malloc(sizeof(bpb_t));
    ata_pio_read48(0, 1, bpb); // Assuming BPB is read correctly into bpb

    // Validate FAT16 signature and filesystem type
    if (bpb->signature != 0xAA55) {
        kprintf("Failed to init FAT16, signature is wrong.\n");
        return false;
    }
    if (strcmp(bpb->fs_type, "FAT16") == 0) {
        kprintf("Failed to init FAT16, wrong fs type.\n");
        return false;
    }

    // Display volume label
    char volume_label[12];
    memcpy(volume_label, bpb->volume_label, 11);
    volume_label[11] = '\0'; // Null-terminate the volume label string
    kprintf("Loaded FAT16 drive %s\n", volume_label);

    // Calculate Root Directory start sector and size
    uint32_t root_dir_start = bpb->reserved_sectors +
                              (bpb->number_of_fat_copies * bpb->sectors_per_fat);
    uint32_t root_dir_size = (bpb->root_entries_count * ENTRY_SIZE) / bpb->bytes_per_sector;

    // Read the first sector of the Root Directory into buffer
    char buffer[512];
    ata_pio_read48(root_dir_start, 1, buffer);

    dir_t *dir = malloc(sizeof(dir_t));;
    memcpy(dir, buffer, sizeof(dir_t));

    printf("Root file: %s\n", dir->Name);
    //printf("Root size: %d\n", dir->FileSize);

    //fat_list_root_files(bpb);

    fat_find_file(bpb, "/ETC/MOTD", dir);

    // Optional: Dump buffer for verification
    //kdumpmem(buffer, 256);

    return true;
}

bool fat_list_root_files(bpb_t *bpb) {
    // Assuming BPB structure is already initialized in `bpb`
    uint32_t root_dir_start = bpb->reserved_sectors +
                              (bpb->number_of_fat_copies * bpb->sectors_per_fat);
    uint32_t root_dir_size = (bpb->root_entries_count * 32) / bpb->bytes_per_sector;
    
    // Allocate a buffer to hold the entire root directory
    char *root_dir_buffer = malloc(root_dir_size * bpb->bytes_per_sector);
    if (!root_dir_buffer) {
        kprintf("Failed to allocate memory for root directory\n");
        return false;
    }

    // Read the entire root directory into the buffer
    for (uint32_t i = 0; i < root_dir_size; i++) {
        ata_pio_read48(root_dir_start + i, 1, root_dir_buffer + (i * bpb->bytes_per_sector));
    }

    // Iterate through each directory entry
    for (uint32_t i = 0; i < bpb->root_entries_count; i++) {
        dir_t *entry = (dir_t *)(root_dir_buffer + (i * sizeof(dir_t)));

        // Check if the entry is a valid file or directory entry
        if (entry->Name[0] == 0x00) {
            break; // End of directory
        }
        if (entry->Name[0] == 0xE5 || (entry->Attr & 0x0F) == 0x0F) {
            continue; // Skip deleted or long file name entries
        }

        // Null-terminate and print the file name
        char file_name[12];
        memcpy(file_name, entry->Name, 11);
        file_name[11] = '\0';

        kprintf("File: %s, Size: %d bytes\n", file_name, entry->FileSize);
    }

    free(root_dir_buffer); // Free allocated buffer
    return true;
}

bool fat_find_in_root_directory(bpb_t *bpb, const char *name, dir_t *result) {
    // Assuming BPB structure is already initialized in `bpb`
    uint32_t root_dir_start = bpb->reserved_sectors +
                              (bpb->number_of_fat_copies * bpb->sectors_per_fat);
    uint32_t root_dir_size = (bpb->root_entries_count * 32) / bpb->bytes_per_sector;

    uint32_t entries_per_sector = bpb->bytes_per_sector / sizeof(dir_t);
    char buffer[512];
    
    for (uint32_t sector = 0; sector < root_dir_size; sector++) {
        ata_pio_read48(root_dir_start + sector, 1, buffer);

        for (uint32_t i = 0; i < entries_per_sector; i++) {
            dir_t *entry = (dir_t *)(buffer + i * sizeof(dir_t));

            if (entry->Name[0] == 0x00) return false; // End of entries
            if (entry->Name[0] == 0xE5) continue;     // Deleted entry

            char entry_name[12];
            memcpy(entry_name, entry->Name, 11);
            entry_name[11] = '\0';

            if (strcmp(entry_name, name) == 0) {
                memcpy(result, entry, sizeof(dir_t));
                return true;
            }
        }
    }
    return false;
}


bool fat_find_file(bpb_t *bpb, const char *path, dir_t *file_entry) {
    // Tokenize the path by '/'
    char *token;
    char path_copy[256];
    strncpy(path_copy, path, sizeof(path_copy));
    token = strtok(path_copy, "/");

    // Start traversal from the root directory, handled separately in FAT16
    uint32_t current_dir_cluster = 0; // 0 indicates we're starting from the root directory

    dir_t entry;
    while (token != NULL) {
        if (current_dir_cluster == 0) {
            // We're in the root directory, handle it by reading `root_dir_start`
            if (!fat_find_in_root_directory(bpb, token, &entry)) {
                kprintf("File or directory '%s' not found in root\n", token);
                return false;
            }
        } else {
            // For subdirectories, search within the directory clusters
            if (!fat_find_in_directory(bpb, current_dir_cluster, token, &entry)) {
                kprintf("File or directory '%s' not found\n", token);
                return false;
            }
        }

        // Check if this is the final file or if we need to move to a subdirectory
        if ((entry.Attr & DIR_ATTR_DIRECTORY) == 0) {
            memcpy(file_entry, &entry, sizeof(dir_t));
            return true;  // File found
        }

        // Move to the next directory level by updating current cluster
        current_dir_cluster = entry.FstClusHI; // OR LO
        token = strtok(NULL, "/");
    }
    
    return false; // File not found
}

uint32_t get_cluster_sector(bpb_t *bpb, uint32_t cluster_number) {
    uint32_t data_region_start = bpb->reserved_sectors +
                                 (bpb->number_of_fat_copies * bpb->sectors_per_fat) +
                                 ((bpb->root_entries_count * 32) / bpb->bytes_per_sector);

    return data_region_start + (cluster_number - 2) * bpb->sectors_per_claster;
}

uint32_t get_next_cluster(dir_t *entry) {
    return (entry->FstClusHI << 16) | entry->FstClusLO;
}

bool fat_find_in_directory(bpb_t *bpb, uint32_t cluster, const char *name, dir_t *result) {
    uint32_t dir_start = get_cluster_sector(bpb, cluster);
    uint32_t entries_per_sector = bpb->bytes_per_sector / sizeof(dir_t);
    char buffer[512];

    for (uint32_t sector = 0; sector < bpb->sectors_per_claster; sector++) { // Fixed: iterate through sectors in cluster
        ata_pio_read48(dir_start + sector, 1, buffer);

        for (uint32_t i = 0; i < entries_per_sector; i++) {
            dir_t *entry = (dir_t *)(buffer + i * sizeof(dir_t));

            if (entry->Name[0] == 0x00) return false; // End of entries
            if (entry->Name[0] == 0xE5) continue;     // Deleted entry

            char entry_name[12];
            memcpy(entry_name, entry->Name, 11);
            entry_name[11] = '\0';

            if (strcmp(entry_name, name) == 0) {
                memcpy(result, entry, sizeof(dir_t));
                return true;
            }
        }
    }
    return false;
}