// Advanced Technology Attachment

#define PRIMARY_BASE 0x1F0
#define PRIMARY_CTRL 0x3F6
#define SECONDARY_BASE 0x170
#define SECONDARY_CTRL 0x376

#define ATA_READ  0x20
#define ATA_WRITE 0x30

/* ATA Status Register Bits */
#define ATA_STATUS_BSY  0x80  /* Busy */
#define ATA_STATUS_DRDY 0x40  /* Drive Ready */
#define ATA_STATUS_DRQ  0x08  /* Data Request */
#define ATA_STATUS_ERR  0x01  /* Error */

#include <stdint.h>

/* lba: Logical Block Addressing */
void ata_read(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count, void *buffer);

/* lba: Logical Block Addressing */
void ata_write(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count, const void *buffer);

void set_ata_handler_proc(void (*proc)(uint8_t));

void ata_handler();

void apic_ata_handler();