// Advanced Technology Attachment

#define PRIMARY_BASE 0x1F0
#define PRIMARY_CTRL 0x3F6
#define SECONDARY_BASE 0x170
#define SECONDARY_CTRL 0x376

#define ATA_READ  0x20
#define ATA_WRITE 0x30

#include <stdint.h>

/* lba: Logical Block Addressing */
void ata_read(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count, void *buffer);

/* lba: Logical Block Addressing */
void ata_write(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count, const void *buffer);