#include "file_system.h"

static struct{
    file_entity_t* file;
    char* buffer;
    uint32_t sector_count;
    uint32_t size;
    SUCC_ERR_V
}args;

static void file_write_error(){
    disk_close(args.error_proc, args.error_proc);
}

static void file_write_close_disk(){
    disk_close(args.success_proc, args.error_proc);
}

static void file_write_update_file(){
    ata_write_sync(PRIMARY_BASE, 0, args.file->fs.lba, 1, args.file, file_write_close_disk, file_write_error);
}

static void file_write_data(){
    file_entity_t* file = args.file;
    if(file->data && file->n_blocks){
        disk_free(file->data, file->n_blocks);
    }
    file->data = disk_alloc(args.sector_count);
    if(!file->data){
        file_write_error();
        return;
    }
    file->n_blocks = args.sector_count;
    file->size = args.size;
    ata_write_sync(PRIMARY_BASE, 0,  file->data, file->n_blocks, args.buffer, file_write_update_file, file_write_error);
}

void file_write(file_entity_t* file, char* buffer, uint32_t sector_count, uint32_t size, SUCC_ERR){
    if(file->fs.type != FILE_TYPE){
        if(error_proc)
            error_proc();
        return;
    }
    args.file = file;
    args.buffer = buffer;
    args.sector_count = sector_count;
    args.size = size;
    args.success_proc = success_proc;
    args.error_proc = error_proc;

    disk_open(file_write_data, error_proc);
}
