#pragma once
#include <types.h>
#include <drv/ata.h>
#include <misc/string.h>
#include <misc/memory.h>
#include <misc/malloc.h>
#include <kernel.h>

typedef struct {
    uint16_t bytes_per_sector;          // IMPORTANT
    uint8_t sectors_per_cluster;        // IMPORTANT
    uint16_t reserved_sectors;          // IMPORTANT
    uint8_t FAT_count;                  // IMPORTANT
    uint16_t dir_entries;
    uint16_t total_sectors;
    uint8_t media_descriptor_type;
    uint16_t count_sectors_per_FAT12_16; // FAT12/FAT16 only.
    uint16_t count_sectors_per_track;
    uint16_t count_heads_or_sizes_on_media;
    uint32_t count_hidden_sectors;
    uint32_t large_sectors_on_media;  // This is set instead of total_sectors if it's > 65535

    // Extended Boot Record
    uint32_t count_sectors_per_FAT32;   // IMPORTANT
    uint16_t flags;
    uint16_t FAT_version;
    uint32_t cluster_number_root_dir;   // IMPORTANT
    uint16_t sector_number_FSInfo;
    uint16_t sector_number_backup_boot_sector;
    uint8_t drive_number;
    uint8_t windows_flags;
    uint8_t signature;                  // IMPORTANT
    uint32_t volume_id;
    char volume_label[12];
    char system_id[9];
} bios_parameter_block;

#define READONLY  1
#define HIDDEN    (1 << 1)
#define SYSTEM    (1 << 2)
#define VolumeID  (1 << 3)
#define DIRECTORY (1 << 4)
#define ARCHIVE   (1 << 5)
#define LFN (READONLY | HIDDEN | SYSTEM | VolumeID)

typedef struct {
    char *name;
    uint8_t dir_attrs;
    uint32_t first_cluster;
    uint32_t file_size;
} dir_entry;

typedef struct {
    uint32_t cluster;
    dir_entry *entries;
    uint32_t num_entries;
} directory;

// End Of Chain
#define EOC 0x0FFFFFF8

typedef struct {
    //FILE *f;
    uint32_t *FAT;
    bios_parameter_block bpb;
    uint32_t partition_begin_sector;
    uint32_t fat_begin_sector;
    uint32_t cluster_begin_sector;
    uint32_t cluster_size;
    uint32_t cluster_alloc_hint;
} f32;

bool fat_init();
f32 *fat_open(char *path);
void fat_close(f32* fs);

// internal
void populate_dir(f32 *fs, directory *dir, uint32_t cluster);
void populate_root_dir(f32 *fs, directory *dir);