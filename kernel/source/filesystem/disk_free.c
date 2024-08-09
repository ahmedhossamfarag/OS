#include "disk.h"
#include "memory.h"
#include "ata.h"
#include "libc.h"

extern uint32_t DiskSize;


extern disk_args_t disk_args;

extern void disk_remove_free_block(disk_freeblock_t* free_block);

void disk_free_merge(disk_freeblock_t* free_block){
    disk_map_t* map = disk_args.map;
    if(free_block->lba + free_block->size == map->free_head){
        map->free_head = free_block->lba;
        map->free_size += free_block->size;
        disk_remove_free_block(free_block);
        return;
    }
    for (disk_freeblock_t* f = disk_args.free_arr; f < disk_args.free_arr + map->n_free_blocks; f++){
        if(free_block->lba + free_block->size == f->lba){
            free_block->size += f->size;
            disk_remove_free_block(f);
            return;
        }
    }
    
    
}

uint8_t disk_grow_free_arr(){
    disk_map_t* map = disk_args.map;

    if(disk_args.free_arr){
        disk_freeblock_t* free_arr = (disk_freeblock_t*) alloc(map->n_blocks*SectorSize);
        if(free_arr){
            mem_copy((char*)disk_args.free_arr, (char*)free_arr, disk_args.free_arr_size*SectorSize);
            free((char*)disk_args.free_arr, disk_args.free_arr_size * SectorSize);
            disk_args.free_arr = free_arr;
            disk_args.free_arr_size = map->n_blocks;
            return 1;
        }
    }else{
        disk_args.free_arr = (disk_freeblock_t*) alloc(SectorSize);
        if(disk_args.free_arr){
            disk_args.free_arr_size = 1;
            return 1;
        }
    }
    return 0;
}

void disk_add_free_block(uint32_t lba, uint32_t size){
    disk_map_t* map = disk_args.map;
    map->n_free_blocks ++;
    map->n_blocks = n_blocks(map->n_free_blocks);

    if(map->n_blocks > disk_args.free_arr_size){
        if(!disk_grow_free_arr())
            return;

    }
    disk_freeblock_t free_block = {lba, size};
    disk_args.free_arr[map->n_free_blocks-1] = free_block;
}


void disk_free(uint32_t lba, uint32_t size){
    disk_map_t* map = disk_args.map;

    if(!size)
        return;
    if(lba < FirstBlockLBA || lba > DiskSize - map->n_blocks - 1)
        return;

    if(!disk_args.map->n_blocks){
        if(lba + size == map->free_head){
            map->free_head = lba;
            map->free_size += size;
            return;
        }
        disk_add_free_block(lba, size);
        return;
    }
    
    for (disk_freeblock_t* f = disk_args.free_arr; f < disk_args.free_arr + map->n_free_blocks; f++){
        if(f->lba + f->size == lba){
            f->size += size;
            disk_free_merge(f);
            return;
        }
    }

    if(lba + size == map->free_head){
        map->free_head = lba;
        map->free_size += size;
        return;
    }

    for (disk_freeblock_t* f = disk_args.free_arr; f < disk_args.free_arr + map->n_free_blocks; f++){
        if(lba + size == f->lba){
            f->lba = lba;
            f->size += size;
            return;
        }
    }

    disk_add_free_block(lba, size);
}