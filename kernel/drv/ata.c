/* ATA PIO Driver */

#include <drv/ata.h>                                                             

uint8_t ata_identify() {
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_DRIVE_HEAD, 0xA0);
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_SECCOUNT, 0);
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_LBA_LO, 0);
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_LBA_MID, 0);
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_LBA_HI, 0);
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_COMM_REGSTAT, 0xEC);
    outb(ATA_PRIMARY_COMM_REGSTAT, 0xE7);

    // Read the status port. If it's zero, the drive does not exist.
    uint8_t status = inb(ATA_PRIMARY_COMM_REGSTAT);

    //printf("Waiting for status.\n");
    while(status & STAT_BSY) {
        uint32_t i = 0;
        while(1) {
            //printf("Printing stuff %d\n", i);
            i++;
        }
        for(i = 0; i < 0x0FFFFFFF; i++) {}
        //rintf("Checking regstat.\n");
        status = inb(ATA_PRIMARY_COMM_REGSTAT);
    }
    
    if(status == 0) return 0;

    //printf("Status indicates presence of a drive. Polling while STAT_BSY... ");
    while(status & STAT_BSY) {
      //printf("\ninb(ATA_PRIMARY_COMM_REGSTAT);... ");
      status = inb(ATA_PRIMARY_COMM_REGSTAT);
    }
    //printf("Done.\n");

    uint8_t mid = inb(ATA_PRIMARY_LBA_MID);
    uint8_t hi = inb(ATA_PRIMARY_LBA_HI);
    if(mid || hi) {
        // The drive is not ATA. (Who knows what it is.)
        return 0;
    }

    //printf("Waiting for ERR or DRQ.\n");
    // Wait for ERR or DRQ
    while(!(status & (STAT_ERR | STAT_DRQ))) {
        status = inb(ATA_PRIMARY_COMM_REGSTAT);
    }

    if(status & STAT_ERR) {
        // There was an error on the drive. Forget about it.
        return 0;
    }

    //printf("Reading IDENTIFY structure.\n");
    //uint8_t *buff = kmalloc(40960, 0, NULL);
    uint8_t buff[256 * 2];
    insw(ATA_PRIMARY_DATA, buff, 256);
    //printf("Success. Disk is ready to go.\n");
    // We read it!
    return 1;
}

bool ata_pio_read28(uint32_t LBA, uint8_t sectorcount, uint8_t *target) {
    // HARD CODE MASTER (for now)
    outb(ATA_PRIMARY_DRIVE_HEAD, 0xE0 | ((LBA >> 24) & 0x0F));
    outb(ATA_PRIMARY_ERR, 0x00);
    outb(ATA_PRIMARY_SECCOUNT, sectorcount);
    outb(ATA_PRIMARY_LBA_LO, LBA & 0xFF);
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 8) & 0xFF);
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 16) & 0xFF);
    outb(ATA_PRIMARY_COMM_REGSTAT, 0x20);

    uint8_t i;
    for(i = 0; i < sectorcount; i++) {
        // POLL!
        while(1) {
            uint8_t status = inb(ATA_PRIMARY_COMM_REGSTAT);
            if(status & STAT_DRQ) {
                // Drive is ready to transfer data!
                break;
            } else if(status & STAT_ERR) {
                return false;
            }
        } 
        // Transfer the data!
        insw(ATA_PRIMARY_DATA, (void *)target, 256);
        target += 256;
    }

    return true;
}

bool ata_pio_read48(uint64_t LBA, uint16_t sectorcount, uint8_t *target) {
    // HARD CODE MASTER (for now)
    outb(ATA_PRIMARY_DRIVE_HEAD, 0x40);                     // Select master
    outb(ATA_PRIMARY_SECCOUNT, (sectorcount >> 8) & 0xFF ); // sectorcount high
    outb(ATA_PRIMARY_LBA_LO, (LBA >> 24) & 0xFF);           // LBA4
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 32) & 0xFF);          // LBA5
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 40) & 0xFF);           // LBA6
    outb(ATA_PRIMARY_SECCOUNT, sectorcount & 0xFF);         // sectorcount low
    outb(ATA_PRIMARY_LBA_LO, LBA & 0xFF);                   // LBA1
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 8) & 0xFF);           // LBA2
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 16) & 0xFF);           // LBA3
    outb(ATA_PRIMARY_COMM_REGSTAT, 0x24);                   // READ SECTORS EXT


    uint8_t i;
    for(i = 0; i < sectorcount; i++) {
        // POLL!
        while(1) {
            uint8_t status = inb(ATA_PRIMARY_COMM_REGSTAT);
            if(status & STAT_DRQ) {
                // Drive is ready to transfer data!
                break;
            } else if(status & STAT_ERR) {
                return false;
            }
        }
        // Transfer the data!
        insw(ATA_PRIMARY_DATA, (void *)target, 256);
        target += 256;
    }
    return true;
}

bool ata_pio_write48(uint64_t LBA, uint16_t sectorcount, uint8_t *target) {

    // HARD CODE MASTER (for now)
    outb(ATA_PRIMARY_DRIVE_HEAD, 0x40);                     // Select master
    outb(ATA_PRIMARY_SECCOUNT, (sectorcount >> 8) & 0xFF ); // sectorcount high
    outb(ATA_PRIMARY_LBA_LO, (LBA >> 24) & 0xFF);           // LBA4
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 32) & 0xFF);          // LBA5
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 40) & 0xFF);           // LBA6
    outb(ATA_PRIMARY_SECCOUNT, sectorcount & 0xFF);         // sectorcount low
    outb(ATA_PRIMARY_LBA_LO, LBA & 0xFF);                   // LBA1
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 8) & 0xFF);           // LBA2
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 16) & 0xFF);           // LBA3
    outb(ATA_PRIMARY_COMM_REGSTAT, 0x34);                   // READ SECTORS EXT

    uint8_t i;
    for(i = 0; i < sectorcount; i++) {
        // POLL!
        while(1) {
            uint8_t status = inb(ATA_PRIMARY_COMM_REGSTAT);
            if(status & STAT_DRQ) {
                // Drive is ready to transfer data!
                break;
            } else if(status & STAT_ERR) {
                return false;
            }
        }
        // Transfer the data!
        outsw(ATA_PRIMARY_DATA, (void *)target, 256);
        target += 256;
    }

    // Flush the cache.
    outb(ATA_PRIMARY_COMM_REGSTAT, 0xE7);
    // Poll for BSY.
    while(inb(ATA_PRIMARY_COMM_REGSTAT) & STAT_BSY) {}
    
    return true;
}
