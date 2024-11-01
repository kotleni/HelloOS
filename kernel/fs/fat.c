#include <fs/fat.h>

bool fat_init() {
    bpb_t *bpb = (bpb_t*) malloc(sizeof(bpb_t));
    ata_pio_read48(0, 1, bpb); // bpb size is 512

    if(bpb->signature != 0xAA55) {
        kprintf("Failed to init FAT16, signature is wrong.\n");
        return false;
    }

    if(strcmp(bpb->fs_type, "FAT16") == 0) {
        kprintf("Failed to init FAT16, wrong fs type.\n");
        return false;
    }

    char volume_label[12];
    memcpy(volume_label, bpb->volume_label, 11); 
    volume_label[12] = 0x00;
    kprintf("Loaded FAT16 drive %s\n", volume_label);

    return true;
}