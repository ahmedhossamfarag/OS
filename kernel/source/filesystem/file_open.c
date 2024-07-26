#include "file_system.h"
#include "libc.h"
#include "strlib.h"

static struct{
    dir_entity_t* parent;
    fs_entity_t* fs;
    char* res;
    char* name;
    SUCC_ERR_V;
}args;

static void dir_open_free(){
    if(args.parent->fs.lba == RootDirLBA){
        free((char*)args.parent, SectorSize);
    }

    if(args.fs){
        free((char*)args.fs, SectorSize);
    }
}

static void dir_open_error(){
    dir_open_free();
    if(args.error_proc)
        args.error_proc();
}

static void dir_open_success(){
    dir_open_free();
    if(args.success_proc)
        args.success_proc();
}

static void dir_open_read_next(){
    if(str_cmp(args.name, args.fs->name) == 0){
        mem_copy((char*)args.fs, args.res, SectorSize);
        dir_open_success();
        return;
    }
    if(args.fs->next){
        ata_read_sync(PRIMARY_BASE, 0, args.fs->next, 1, args.fs, dir_open_read_next, dir_open_error);
    }else{
        dir_open_error();
    }
}


static void dir_open_read_head(){
    if(args.parent->head){
        args.fs = (fs_entity_t*) alloc(SectorSize);
        if(!args.fs){
            dir_open_error();
            return;
        }
        ata_read_sync(PRIMARY_BASE, 0, args.parent->head, 1, args.fs, dir_open_read_next, dir_open_error);
    }else{
        args.fs = 0;
        dir_open_error();
    }
}

void file_open(dir_entity_t* parent, char* name, fs_entity_t* res, SUCC_ERR){
    args.res =  (char*)res;
    args.name = name;
    args.success_proc = success_proc;
    args.error_proc = error_proc;
    if(!parent){
        args.parent = (dir_entity_t*) alloc(SectorSize);
        if(!args.parent){
            if(error_proc)
                error_proc();
            return;
        }
        ata_read_sync(PRIMARY_BASE, 0, RootDirLBA, 1, args.parent, dir_open_read_head, dir_open_error);
    }else{
        if(parent->fs.type != DIR_TYPE){
            if(error_proc)
                error_proc();
            return;
        }
        args.parent = parent;
        dir_open_read_head();
    }
}
