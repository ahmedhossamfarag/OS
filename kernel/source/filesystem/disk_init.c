#include "disk.h"
#include "memory.h"
#include "ata.h"
#include "info.h"

uint32_t DiskSize;
static uint32_t root_lba;

static void disk_get_size(){
    uint16_t* data = (uint16_t*)alloc(SectorSize);

    ata_identify_drive(PRIMARY_BASE, 0, data);

    uint32_t user_addressable_sectors = data[100] | (data[101] << 16);

    DiskSize = user_addressable_sectors;

    free((char*)data, SectorSize);
}

static void disk_read_gpt(){
    gpt_header_t* gpt = (gpt_header_t*) alloc(SectorSize);

    ata_read(PRIMARY_BASE, 0, 1, 1, gpt);

    gpt_partition_entry_t* p = (gpt_partition_entry_t*) alloc(SectorSize);

    ata_read(PRIMARY_BASE, 0, gpt->partition_entry_lba, 1, p);

    root_lba = p[1].starting_lba;

    if(gpt->last_usable_lba){
        DiskSize = gpt->last_usable_lba + 1;
    }else{
        disk_get_size();
    }


    free((char*)p, SectorSize);

    free((char*)gpt, SectorSize);
}


static void disk_read_mbr(){
    mbr_t* mbr = (mbr_t*) alloc(SectorSize);

    ata_read(PRIMARY_BASE, 0, 0, 1, mbr);

    root_lba = mbr->partition_table[1].start_lba;

    disk_get_size();

    free((char*) mbr, SectorSize);
}

static void disk_set_params(){
    if(info_is_gpt()){
        disk_read_gpt();
    }else{
        disk_read_mbr();
    }
}

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
    disk_set_params();

    disk_map_t* map = (disk_map_t*) alloc(SectorSize);
    if(map){
        ata_read(PRIMARY_BASE, 0, DiskMapLBA, 1, map);
        if(map->identefier != MapIdentefier){
            create_map();
        }
        free((char*)map, SectorSize);
    }
}

uint32_t disk_get_rootlba(){
    return root_lba;
}