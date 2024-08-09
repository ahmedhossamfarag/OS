#include "disk.h"
#include "ata.h"
#include "memory.h"

extern uint32_t DiskSize;

disk_args_t disk_args;

static struct{
    void (*success_proc)();
    void (*error_proc)();
} disk_open_args;

void disk_args_free(){
    free((char*)disk_args.map, SectorSize);
    if(disk_args.free_arr){
        free((char*)disk_args.free_arr, disk_args.free_arr_size * SectorSize);
    }
}

static void disk_open_error(){
    disk_args_free();
    if(disk_open_args.error_proc)
        disk_open_args.error_proc();
}

static void disk_open_success(){
    if(disk_open_args.success_proc)
        disk_open_args.success_proc();
}

static void disk_open_read_map(){
    disk_map_t* map = disk_args.map;
    if(map->n_blocks){
        disk_args.free_arr_size = map->n_blocks;
        disk_args.free_arr = (disk_freeblock_t*) alloc(map->n_blocks * SectorSize);
        if(disk_args.free_arr){
            ata_read_sync(PRIMARY_BASE, 0 , DiskMapLBA - map->n_blocks, map->n_blocks, disk_args.free_arr, disk_open_success, disk_open_error);
        }else{
            disk_open_error();
        }
    }else{
        disk_open_success();
    }
}

void disk_open(void (*success_proc)(), void (*error_proc)()){
    disk_args.map = (disk_map_t*) alloc(SectorSize);
    if(disk_args.map){
        disk_open_args.success_proc = success_proc;
        disk_open_args.error_proc = error_proc;

        disk_args.free_arr = 0;

        ata_read_sync(PRIMARY_BASE, 0, DiskMapLBA, 1, disk_args.map, disk_open_read_map, disk_open_error);
    }else{
        if(error_proc)  
            error_proc();
    }
}
