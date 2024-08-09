#include "disk.h"
#include "ata.h"
#include "memory.h"

extern uint32_t DiskSize;


extern disk_args_t disk_args;


static struct{
    void (*success_proc)();
    void (*error_proc)();
} disk_close_args;

extern void disk_args_free();

static void disk_close_error(){
    disk_args_free();
    if(disk_close_args.error_proc)
        disk_close_args.error_proc();
}

static void disk_close_success(){
    disk_args_free();
    if(disk_close_args.success_proc)
        disk_close_args.success_proc();
}


void disk_close_save_map(){
    ata_write_sync(PRIMARY_BASE, 0, DiskMapLBA, 1, disk_args.map, disk_close_success, disk_close_error);
}

void disk_close(void (*success_proc)(), void (*error_proc)()){
    disk_close_args.success_proc = success_proc;
    disk_close_args.error_proc = error_proc;

    disk_map_t* map = disk_args.map;
    if(disk_args.free_arr && map->n_blocks){
        ata_write_sync(PRIMARY_BASE, 0 , DiskMapLBA - map->n_blocks, map->n_blocks, disk_args.free_arr, disk_close_save_map, disk_close_error);
    }else{
        disk_close_save_map();
    }

}
