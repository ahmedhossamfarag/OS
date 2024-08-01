#include "file_system.h"
#include "libc.h"

static struct{
    dir_entity_t* parent;
    fs_entity_t* fs;
    char* res;
    SUCC_ERR_V;
}args;

static void dir_list_free(){
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
    if(!parent || !file_is_open((fs_entity_t*)parent)){
        if(error_proc)
            error_proc();
        return;
    }else{        
        if(parent->fs.type != DIR_TYPE){
            if(error_proc)
                error_proc();
            return;
        }
        args.parent = parent;
        dir_list_read_head();
    }
}
