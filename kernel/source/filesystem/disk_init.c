#include "disk.h"
#include "memory.h"
#include "ata.h"


static void create_map(){
    disk_map_t* map = (disk_map_t*) alloc(SectorSize);
    if(map){
        map->identefier = MapIdentefier;
        map->n_blocks = 0;
        map->n_free_blocks = 0;
        map->free_head = FirstBlockLBA;
        map->free_size = DiskSize - FirstBlockLBA - 1;
        ata_write(PRIMARY_BASE, 0, DiskMapLBA, 1, map);
        free((char*)map, SectorSize);
    }
}

void disk_init() {
    disk_map_t* map = (disk_map_t*) alloc(SectorSize);
    if(map){
        ata_read(PRIMARY_BASE, 0, DiskMapLBA, 1, map);
        if(map->identefier != MapIdentefier){
            create_map();
        }
        free((char*)map, SectorSize);
    }
}