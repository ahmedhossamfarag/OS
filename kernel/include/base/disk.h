#pragma once

#define RootDirLBA 201
#define FirstBlockLBA 300
#define MapIdentefier 0xAF55
#define SectorSize 512
#define DiskMapLBA (DiskSize - 1)

#include <stdint.h>
#include "math.h"
typedef struct
{
    uint32_t lba;
    uint32_t size;
}__attribute__((packed)) disk_freeblock_t;

typedef struct{
    uint16_t identefier;
    uint32_t n_blocks;
    uint64_t n_free_blocks;
    uint32_t free_head;
    uint32_t free_size;
}__attribute__((packed)) disk_map_t;

typedef struct
{
    disk_map_t* map;
    disk_freeblock_t* free_arr;
    uint32_t free_arr_size;
} disk_args_t;

#define n_blocks(n_free_blocks) math_ciel(((float)n_free_blocks * sizeof(disk_freeblock_t)/SectorSize))

void disk_init();

void disk_open(void (*success_proc)(), void (*error_proc)());

uint32_t disk_alloc(uint32_t size);

void disk_free(uint32_t lba, uint32_t size);

void disk_close(void (*success_proc)(), void (*error_proc)());

// Master Boot Record (MBR) structure
typedef struct {
    uint8_t boot_code[446];           // Boot code area
    struct {
        uint8_t status;               // Bootable flag
        uint8_t start_head;           // Starting head
        uint16_t start_sector : 6;     // Starting sector
        uint16_t start_cylinder : 10;  // Starting cylinder
        uint8_t type;                 // Partition type
        uint8_t end_head;             // Ending head
        uint16_t end_sector : 6;       // Ending sector
        uint16_t end_cylinder : 10;    // Ending cylinder
        uint32_t start_lba;           // Starting LBA (for CHS partitions, this is 0)
        uint32_t size;                // Size of the partition in sectors
    } partition_table[4];             // Partition table entries (4 entries)
    uint16_t signature;               // Signature (0xAA55)
}__attribute__((packed)) MBR;

// GPT Header structure
typedef struct {
    uint8_t signature[8];             // "EFI PART" signature
    uint32_t revision;                // GPT revision (0x00010000 for version 1.0)
    uint32_t header_size;            // Size of the GPT header
    uint32_t header_crc32;            // CRC32 checksum of the GPT header
    uint32_t reserved;                // Reserved, must be zero
    uint64_t current_lba;             // Current GPT header LBA
    uint64_t backup_lba;              // Backup GPT header LBA
    uint64_t first_usable_lba;        // First LBA usable by partitions
    uint64_t last_usable_lba;         // Last LBA usable by partitions
    uint8_t disk_guid[16];            // Disk GUID
    uint64_t partition_entry_lba;     // LBA where partition entries start
    uint32_t num_partition_entries;   // Number of partition entries
    uint32_t partition_entry_size;    // Size of each partition entry
    uint32_t partition_array_crc32;   // CRC32 checksum of the partition entries array
}__attribute__((packed)) GPTHeader;

// GPT Partition Entry structure
typedef struct {
    uint8_t partition_type_guid[16];  // Partition type GUID
    uint8_t unique_partition_guid[16];// Unique partition GUID
    uint64_t starting_lba;            // Starting LBA of the partition
    uint64_t ending_lba;              // Ending LBA of the partition
    uint64_t attributes;              // Attributes of the partition
    uint16_t partition_name[36];      // Partition name (UTF-16, 72 bytes total)
}__attribute__((packed)) GPTPartitionEntry;
