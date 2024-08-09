#include "disk.h"
#include "ata.h"
#include "memory.h"

extern disk_args_t disk_args;


void disk_remove_free_block(disk_freeblock_t* free_block){
    disk_map_t* map = disk_args.map;

    for (disk_freeblock_t* f = free_block; f <  disk_args.free_arr + map->n_free_blocks - 1; f++)
    {
        *f = *(f+1);
    }
    map->n_free_blocks --;
    map->n_blocks = n_blocks(map->n_free_blocks);
}

uint32_t disk_alloc_head(uint32_t size){
    disk_map_t* map = disk_args.map;
    if(map->free_size > size){
        uint32_t lba = map->free_head;
        map->free_head += size;
        map->free_size -= size;
        return lba;
    }else{
        return 0;
    }
}

uint32_t disk_alloc(uint32_t size){
    if(!size)
        return 0;

    disk_map_t* map = disk_args.map;
    uint32_t lba;

    if(!disk_args.map->n_blocks){
        return disk_alloc_head(size);
    }

    disk_freeblock_t* free_block = 0;
    for (disk_freeblock_t* f = disk_args.free_arr; f < disk_args.free_arr + map->n_free_blocks; f++)
    {
        if(f->size >= size){
            free_block = f;
            break;
        }
    }
    
    if(!free_block){
        return disk_alloc_head(size);
    }

    if(free_block->size > size){
        lba = free_block->lba;
        free_block->lba += size;
        free_block->size -= size;
        return lba;
    }

    if(free_block->size == size){
        lba = free_block->lba;
        disk_remove_free_block(free_block);
        return lba;
    }
    
    return 0;
}