#pragma once

#define RootDirLBA 201
#define FirstBlockLBA 300
#define MapIdentefier 0xAF55
#define SectorSize 512
#define DiskSize 1000
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