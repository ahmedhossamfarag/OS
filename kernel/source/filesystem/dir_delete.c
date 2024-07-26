#include "file_system.h"
#include "dslib.h"

static struct{
    dir_entity_t* parent;
    dir_entity_t* dir;
    fs_entity_t* fs;
    list_t* list;
    SUCC_ERR_V
}args;

static void dir_delete_free(){
    if(args.parent){
        free((char*)args.parent, SectorSize);
    }
    if(args.fs){
        free((char*)args.fs, SectorSize);
    }
    list_delete(args.list);
}

static void dir_delete_error(){
    dir_delete_free();
    disk_close(args.error_proc, args.error_proc);
}

static void dir_delete_success(){
    dir_delete_free();
    disk_close(args.success_proc, args.error_proc);
}

static void dir_delete_file(file_entity_t* file){
    if(file->data && file->n_blocks){
        disk_free(file->data, file->n_blocks);
    }
    disk_free(file->fs.lba, 1);

}

static void dir_delete_dir(dir_entity_t* dir){
    disk_free(dir->fs.lba, 1);
    if(dir->head){
        list_append(args.list, (void*) dir->head);
    }
}

static void dir_delete_next(){
    fs_entity_t* fs = args.fs;
    if(fs->type == FILE_TYPE){
        dir_delete_file((file_entity_t*)fs);
    }else if(fs->type == DIR_TYPE){
        dir_delete_dir((dir_entity_t*)fs);
    }else{
        dir_delete_error();
    }

    uint32_t next = fs->next;
    if(!next){
        next = (uint32_t) list_remove_first(args.list);
    }

    if(!next){
        dir_delete_success();
    }else{
        ata_read_sync(PRIMARY_BASE, 0, next, 1, args.fs, dir_delete_next, dir_delete_error);
    }
}

static void dir_delete_fs(){
    if(args.dir->head){
        args.fs = (fs_entity_t*) alloc(SectorSize);
        if(!args.fs){
            dir_delete_error();
            return;
        }
        ata_read_sync(PRIMARY_BASE, 0, args.dir->head, 1, args.fs, dir_delete_next, dir_delete_error);
    }else{
        dir_delete_success();
    }
}

static void dir_dir_remove(){
    dir_remove(args.parent, (fs_entity_t*)args.dir, dir_delete_fs, dir_delete_error);
}

static void dir_delete_lba(){
    dir_entity_t* dir = args.dir;

    disk_free(dir->fs.lba, 1);

    if(dir->fs.parent != dir->fs.parent != RootDirLBA){
        args.parent = (dir_entity_t*) alloc(SectorSize);
        if(!args.parent){
            dir_delete_error();
            return;
        }
        ata_read_sync(PRIMARY_BASE, 0, dir->fs.parent, 1, args.parent, dir_dir_remove, dir_delete_error);
    }else{
        args.parent = 0;
        dir_dir_remove();
    }
}

void dir_delete(dir_entity_t* dir, SUCC_ERR){
    if(dir->fs.type != DIR_TYPE){
        if(error_proc)
            error_proc();
        return;
    }

    args.parent = 0;
    args.dir = dir;
    args.success_proc = success_proc;
    args.error_proc = error_proc;
    args.list = list_new(alloc, free);
    if(args.list){
       if(error_proc)
            error_proc();
        return; 
    }

    disk_open(dir_delete_lba, error_proc);
}