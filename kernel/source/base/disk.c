#include "disk.h"
#include "memory.h"
#include "ata.h"


static void create_map(uint32_t lba, DiskFreeBlock first_block){
    DiskMap* map = (DiskMap*) alloc(SectorSize);
    if(map){
        map->identefier = MapIdentefier;
        map->next_map = 0;
        for (DiskFreeBlock* b = map->free_blocks; b < map->free_blocks + Map_N_Blocks; b++)
        {
            b->lba = 0;
            b->size = 0;
        }
        map->n_blocks = 1;
        map->free_blocks[0] = first_block;
        ata_write(PRIMARY_BASE, 0, lba, 1, map);
        free((char*)map, SectorSize);
    }
}

void disk_init() {
    DiskMap* map = (DiskMap*) alloc(SectorSize);
    if(map){
        ata_read(PRIMARY_BASE, 0, FirstMapLBA, 1, map);
        if(map->identefier != MapIdentefier){
            DiskFreeBlock first_block = {FirstBlockLBA, DiskSize};
            create_map(FirstMapLBA, first_block);
        }
        free((char*)map, SectorSize);
    }
}

uint32_t disk_alloc(uint32_t size) {
    if(size == 0) return 0;

    DiskMap* map = (DiskMap*) alloc(SectorSize);
    uint32_t lba = 0;
    if(map){
        uint32_t map_lba = FirstMapLBA;
        DiskFreeBlock* free_block = NULL;
        do{
            ata_read(PRIMARY_BASE, 0, map_lba, 1, map);
            for (DiskFreeBlock* b = map->free_blocks; b < map->free_blocks + map->n_blocks; b++)
            {
                if(b->size >= size){
                    free_block = b;
                    break;
                }
            }
            if(free_block) break;
            map_lba = map->next_map;
        }while(map_lba);
        if(free_block){
            lba = free_block->lba;
            if(free_block->size > size){
                free_block->lba += size;
                free_block->size -= size;
            }else{
                for (DiskFreeBlock* b = free_block + 1; b < map->free_blocks + map->n_blocks-1; b++)
                {
                    *b = *(b+1);
                }
                map->n_blocks -= 1;
            }
            ata_write(PRIMARY_BASE, 0, map_lba, 1, map);
        }
        free((char*)map, SectorSize);
    }
    return lba;
}

void disk_free(uint32_t lba, uint32_t size) {
    if(lba < FirstBlockLBA || lba >= DiskSize || size == 0)    return;

    DiskFreeBlock free_block = {lba, size};
    DiskMap* map = (DiskMap*) alloc(SectorSize);
    if(map){
        uint32_t map_lba = FirstMapLBA;
        do{
            ata_read(PRIMARY_BASE, 0, map_lba, 1, map);
            if(map->n_blocks < Map_N_Blocks){
                break;
            }
            map_lba = map->next_map;
        }while(map_lba);

        if(map->n_blocks < Map_N_Blocks){
            uint8_t done = 0;
            for (DiskFreeBlock* b = map->free_blocks; b < map->free_blocks + map->n_blocks; b++){
                if(b->lba+b->size == lba){
                    b->size += size;
                    done = 1;
                    break;
                }
                if(lba+size == b->lba){
                    b->lba = lba;
                    b->size += size;
                    done = 1;
                    break;
                }
            }
            if(!done){
                *(map->free_blocks+map->n_blocks) = *(map->free_blocks+map->n_blocks-1);
                *(map->free_blocks+map->n_blocks-1) = free_block;
                map->n_blocks += 1;
            }
        }else{
            DiskFreeBlock base_block = map->free_blocks[map->n_blocks-1];
            create_map(lba, base_block);
            map->next_map = lba;
            free_block.lba ++;
            free_block.size --;
            if(free_block.size){
                *(map->free_blocks+map->n_blocks-1) = free_block;
            }
            else{
                map->n_blocks--;
            }
        }
        ata_write(PRIMARY_BASE, 0, map_lba, 1, map);
        free((char*)map, SectorSize);
    }

}
