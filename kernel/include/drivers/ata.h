// Advanced Technology Attachment

#define PRIMARY_BASE 0x1F0
#define PRIMARY_CTRL 0x3F6
#define SECONDARY_BASE 0x170
#define SECONDARY_CTRL 0x376

#define ATA_READ  0x20
#define ATA_WRITE 0x30
#define ATA_IDENTIFY_DRIVE 0xEC

/* ATA Status Register Bits */
#define ATA_STATUS_BSY  0x80  /* Busy */
#define ATA_STATUS_DRDY 0x40  /* Drive Ready */
#define ATA_STATUS_DRQ  0x08  /* Data Request */
#define ATA_STATUS_ERR  0x01  /* Error */

#include <stdint.h>

void ata_init();

/* lba: Logical Block Addressing */
void ata_read(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count, void *buffer);

/* lba: Logical Block Addressing */
void ata_write(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count, const void *buffer);


void ata_read_sync(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count, void *buffer, void (*success_proc)(), void (*error_proc)());

void ata_write_sync(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count, void *buffer, void (*success_proc)(), void (*error_proc)());


void ata_handler();

void apic_ata_handler();

void ata_identify_drive(uint16_t base, uint8_t drive,  void *buffer);