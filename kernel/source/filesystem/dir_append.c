#include "file_system.h"

static struct{
    dir_entity_t* parent;
    fs_entity_t* tail;
    fs_entity_t* fs;
    SUCC_ERR_V;

}args;

static void dir_append_free(){
    if(args.parent->fs.lba == RootDirLBA){
        free((char*)args.parent, SectorSize);
    }
    if(args.tail){
        free((char*)args.tail, SectorSize);
    }
}

static void dir_append_error(){
    dir_append_free();
    if(args.error_proc)
        args.error_proc();
}

static void dir_append_success(){
    dir_append_free();
    if(args.success_proc)
        args.success_proc();
}

static void dir_append_update_parent(){
    ata_write_sync(PRIMARY_BASE, 0, args.parent->fs.lba, 1, args.parent, dir_append_success, dir_append_error);
}

static void dir_append_update_tail(){
    if(args.tail){
        ata_write_sync(PRIMARY_BASE, 0, args.tail->lba, 1, args.tail, dir_append_update_parent, dir_append_error);
    }else{
        dir_append_update_parent();
    }
}

static void dir_append_update_fs(){
    ata_write_sync(PRIMARY_BASE, 0, args.fs->lba, 1, args.fs, dir_append_update_tail, dir_append_error);
}

static void dir_append_fs(){
    dir_entity_t* parent = args.parent;
    fs_entity_t* tail = args.tail;
    fs_entity_t* fs = args.fs;

    fs->parent = parent->fs.lba;
    if(!tail){
        parent->head = parent->tail = fs->lba;
        fs->prev = fs->next = 0;
        parent->n_childs = 1;
    }else{
        tail->next = fs->lba;
        fs->prev = tail->lba;
        fs->next = 0;
        parent->tail = fs->lba;
        parent->n_childs ++;
    }
    dir_append_update_fs();
}

static void dir_append_read_tail(){
    if(args.parent->tail){
        args.tail = (fs_entity_t*) alloc(SectorSize);
        if(!args.tail){
            dir_append_error();
            return;
        }
        ata_read_sync(PRIMARY_BASE, 0, args.parent->tail, 1, args.tail, dir_append_fs, dir_append_error);
    }else{
        args.tail = 0;
        dir_append_fs();
    }
}

void dir_append(dir_entity_t* parent, fs_entity_t* fs, SUCC_ERR){
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
        ata_read_sync(PRIMARY_BASE, 0, RootDirLBA, 1, args.parent, dir_append_read_tail, dir_append_error);
    }else{
        if(parent->fs.type != DIR_TYPE){
            if(error_proc)
                error_proc();
            return;
        }
        args.parent = parent;
        dir_append_read_tail();
    }
}