#ifndef H_FAT
#define H_FAT

// http://www.maverick-os.dk/FileSystemFormats/FAT16_FileSystem.html

#include <types.h>
#include <drv/ata.h>
#include <misc/string.h>
#include <misc/memory.h>
#include <misc/malloc.h>
#include <kernel.h>

struct bpb {
    uint8_t jmp_bootstrap[3]; // Code to jump to the bootstrap code.
    unsigned char oem_id[8]; // Oem ID - Name of the formatting OS
    uint16_t bytes_per_sector; // Bytes per Sector
    uint8_t sectors_per_claster; // Sectors per Cluster - Usual there is 512 bytes per sector.
    uint16_t reserved_sectors; // Reserved sectors from the start of the volume.

    uint8_t number_of_fat_copies; // Number of FAT copies - Usual 2 copies are used to prevent data loss.
    uint16_t root_entries_count; // Number of possible root entries - 512 entries are recommended.
    uint16_t total_sectors16; // Small number of sectors - Used when volume size is less than 32 Mb.
    uint8_t media_descriptor; // Media Descriptor
    uint16_t sectors_per_fat; // Sectors per FAT
    uint16_t sectors_per_track; // Sectors per Track
    uint16_t num_of_heads; // Number of Heads
    uint32_t hidden_sectors; // Hidden Sectors

    uint32_t total_sectors32; // Large number of sectors - Used when volume size is greater than 32 Mb.
    uint8_t drive_snum; // Drive Number - Used by some bootstrap code, fx. MS-DOS.
    uint8_t reserved; // Is used by Windows NT to decide if it shall check disk integrity.
    uint8_t extended_boot_signature; // Indicates that the next three fields are available.
    uint32_t serial_number; // Volume Serial Number
    unsigned char volume_label[11]; // Volume Label - Should be the same as in the root directory.
    unsigned char fs_type[8]; // File System Type - The string should be 'FAT16'
    uint8_t bootstrap[448]; // Bootstrap code - May schrink in the future.
    uint16_t signature; // Boot sector signature - This is the AA55h signature.
} __attribute__((packed));
typedef struct bpb bpb_t;

struct dir {
    unsigned char Name[11];
    uint8_t Attr;
    uint8_t NTRes;
    uint8_t CrtTimeTenth;
    uint16_t CrtTime;

    uint16_t CrtDate;
    uint16_t LstAccDate;
    uint16_t FstClusHI;
    uint16_t WrtTime;
    uint16_t WrtDate;
    uint16_t FstClusLO;
    uint32_t FileSize;
} __attribute__((packed));
typedef struct dir dir_t;

enum dirattr {
    DIR_ATTR_READONLY = 1 << 0,
    DIR_ATTR_HIDDEN   = 1 << 1,
    DIR_ATTR_SYSTEM   = 1 << 2,
    DIR_ATTR_VOLUMEID = 1 << 3,
    DIR_ATTR_DIRECTORY= 1 << 4,
    DIR_ATTR_ARCHIVE  = 1 << 5,
    DIR_ATTR_LFN      = 0xF
};
typedef enum dirattr dirattr_t;

bool fat_init();


#endif