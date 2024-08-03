#include "file_system.h"

extern fs_entity_t** file_get_open_lba(uint32_t lba);

static struct{
    dir_entity_t* parent;
    file_entity_t* file;
    SUCC_ERR_V
}args;

static void file_delete_free(){
    if(args.parent && !file_is_open((fs_entity_t*)args.parent)){
        free((char*)args.parent, SectorSize);
    }
}

static void file_delete_error(){
    file_delete_free();
    disk_close(args.error_proc, args.error_proc);
}

static void file_delete_success(){
    file_delete_free();
    disk_close(args.success_proc, args.error_proc);
}

static void file_dir_remove(){
    dir_remove(args.parent, (fs_entity_t*)args.file, file_delete_success, file_delete_error);
}

static void file_delete_lba(){
    file_entity_t* file = args.file; 
    if(file->data && file->n_blocks){
        disk_free(file->data, file->n_blocks);
    }
    file_close((fs_entity_t*)file, 0, 0);
    disk_free(file->fs.lba, 1);

    fs_entity_t** parent_pntr = file_get_open_lba(file->fs.parent);
    if(parent_pntr){
        args.parent = (dir_entity_t*) *parent_pntr;
        file_dir_remove();
    }else{
        args.parent = (dir_entity_t*) alloc(SectorSize);
        if(!args.parent){
            file_delete_error();
            return;
        }
        ata_read_sync(PRIMARY_BASE, 0, file->fs.parent, 1, args.parent, file_dir_remove, file_delete_error);
    }
}

void file_delete(file_entity_t* file, SUCC_ERR){
    if(!file_is_open((fs_entity_t*)file) || file->fs.type != FILE_TYPE){
        if(error_proc)
            error_proc();
        return;
    }

    args.parent = 0;
    args.file = file;
    args.success_proc = success_proc;
    args.error_proc = error_proc;

    disk_open(file_delete_lba, error_proc);
}