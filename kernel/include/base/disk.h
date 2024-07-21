#define FirstMapLBA  70
#define RootDirLBA 71
#define FirstBlockLBA 72
#define MapIdentefier 0xAF55
#define Map_N_Blocks 60
#define SectorSize 512
#define DiskSize 1000

#include <stdint.h>

typedef struct
{
    uint32_t lba;
    uint32_t size;
}__attribute__((packed)) disk_freeblock_t;

typedef struct{
    uint16_t identefier;
    uint32_t next_map;
    uint8_t n_blocks;
    disk_freeblock_t free_blocks[Map_N_Blocks];
}__attribute__((packed)) disk_map_t;


void disk_init();

uint32_t disk_alloc(uint32_t size);

void disk_free(uint32_t lba, uint32_t size);