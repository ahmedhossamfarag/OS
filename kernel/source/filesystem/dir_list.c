#include "file_system.h"
#include "libc.h"

static struct{
    uint8_t remove_parent;
    dir_entity_t* parent;
    fs_entity_t* fs;
    char* res;
    SUCC_ERR_V;
}args;

static void dir_list_free(){
    if(args.remove_parent){
        free((char*)args.parent, SectorSize);
    }

    if(args.fs){
        free((char*)args.fs, SectorSize);
    }
}

static void dir_list_error(){
    dir_list_free();
    if(args.error_proc)
        args.error_proc();
}

static void dir_list_success(){
    dir_list_free();
    if(args.success_proc)
        args.success_proc();
}

static void dir_list_read_next(){
    mem_copy((char*)args.fs, args.res, SectorSize);
    args.res += SectorSize;
    if(args.fs->next){
        ata_read_sync(PRIMARY_BASE, 0, args.fs->next, 1, args.fs, dir_list_read_next, dir_list_error);
    }else{
        dir_list_success();
    }
}


static void dir_list_read_head(){
    if(args.parent->head){
        args.fs = (fs_entity_t*) alloc(SectorSize);
        if(!args.fs){
            dir_list_error();
            return;
        }
        ata_read_sync(PRIMARY_BASE, 0, args.parent->head, 1, args.fs, dir_list_read_next, dir_list_error);
    }else{
        args.fs = 0;
        dir_list_success();
    }
}


void dir_list(dir_entity_t* parent, fs_entity_t* res, SUCC_ERR){
    args.res =  (char*)res;
    args.success_proc = success_proc;
    args.error_proc = error_proc;
    if(!parent){
        args.parent = (dir_entity_t*) alloc(SectorSize);
        if(!args.parent){
            if(error_proc)
                error_proc();
            return;
        }
        args.remove_parent = 0;
        ata_read_sync(PRIMARY_BASE, 0, RootDirLBA, 1, args.parent, dir_list_read_head, dir_list_error);
    }else{        
        if(parent->fs.type != DIR_TYPE){
            if(error_proc)
                error_proc();
            return;
        }
        args.parent = parent;
        args.remove_parent = 1;
        dir_list_read_head();
    }
}
