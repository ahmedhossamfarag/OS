#include "file_system.h"

static struct{
    dir_entity_t* parent;
    fs_entity_t* fs;
    fs_entity_t* next;
    fs_entity_t* prev;
    SUCC_ERR_V;
}args;

static void dir_remove_free(){
    if(args.parent->fs.lba == RootDirLBA){
        free((char*)args.parent, SectorSize);
    }
    if(args.next){
        free((char*)args.next, SectorSize);
    }
    if(args.prev){
        free((char*)args.prev, SectorSize);
    }
}

static void dir_remove_error(){
    dir_remove_free();
    if(args.error_proc)
        args.error_proc();
}

static void dir_remove_success(){
    dir_remove_free();
    if(args.success_proc)
        args.success_proc();
}

static void dir_remove_update_parent(){
    ata_write_sync(PRIMARY_BASE, 0, args.parent->fs.lba, 1, args.parent, dir_remove_success, dir_remove_error);
}

static void dir_remove_update_prev(){
    if(args.prev){
        ata_write_sync(PRIMARY_BASE, 0, args.prev->lba, 1, args.prev, dir_remove_update_parent, dir_remove_error);        
    }else{
        dir_remove_update_parent();
    }
}

static void dir_remove_update_next(){
    if(args.next){
        ata_write_sync(PRIMARY_BASE, 0, args.next->lba, 1, args.next, dir_remove_update_prev, dir_remove_error);
    }else{
        dir_remove_update_prev();
    }
}

static void dir_remove_fs(){
    dir_entity_t* parent = args.parent;
    fs_entity_t* fs = args.fs;
    fs_entity_t* next = args.next;
    fs_entity_t* prev = args.prev;
    if(next){
        next->prev = fs->prev;
    }else{
        parent->tail = fs->prev;
    }

    if(prev){
        prev->next = fs->next;
    }else{
        parent->head = fs->next;
    }

    parent->n_childs --;

    dir_remove_update_next();
}

static void dir_remove_read_prev(){
    if (args.fs->prev)
    {
        args.prev = (fs_entity_t*) alloc(SectorSize);
        if(!args.prev){
            dir_remove_error();
            return;
        }
        ata_read_sync(PRIMARY_BASE, 0, args.fs->prev, 1, args.prev, dir_remove_fs, dir_remove_error);
    }else{
        args.prev = 0;
        dir_remove_fs();
    }
    
}

static void dir_remove_read_next(){
    if(args.fs->next){
        args.next = (fs_entity_t*) alloc(SectorSize);
        if(!args.next){
            dir_remove_error();
            return;
        }
        ata_read_sync(PRIMARY_BASE, 0, args.fs->next, 1, args.next, dir_remove_read_prev, dir_remove_error);
    }else{
        args.next = 0;
        dir_remove_read_prev();
    }
}

void dir_remove(dir_entity_t* parent, fs_entity_t* fs, SUCC_ERR){
    args.fs = fs;
    args.success_proc = success_proc;
    args.error_proc = error_proc;
    if(!parent){
        args.parent = (dir_entity_t*) alloc(SectorSize);
        if(!args.parent){
            if(error_proc)
                error_proc();
                return;
        }
        ata_read_sync(PRIMARY_BASE, 0, RootDirLBA, 1, args.parent, dir_remove_read_next, dir_remove_error);
    }else{
        if(parent->fs.type != DIR_TYPE){
            if(error_proc)
                error_proc();
            return;
        }
        args.parent = parent;
        dir_remove_read_next();
    }
}