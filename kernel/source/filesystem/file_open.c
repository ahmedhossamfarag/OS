#include "file_system.h"
#include "libc.h"
#include "strlib.h"
#include "dslib.h"

array_t* open_files;

static struct{
    dir_entity_t* parent;
    fs_entity_t* fs;
    fs_entity_t** res;
    char* name;
    SUCC_ERR_V;
}args;

static void file_open_error(){
    if(args.fs){
        free((char*)args.fs, SectorSize);
    }
    if(args.error_proc)
        args.error_proc();
}

extern fs_entity_t** file_get_open(fs_entity_t* fs);

static fs_entity_t* file_add_open(){
    fs_entity_t** fs_pntr = file_get_open(args.fs);
    if(fs_pntr){
        free((char*) args.fs, SectorSize);
        return *fs_pntr;
    }else{
        array_add(open_files, args.fs);
        return args.fs;
    }
}

static void file_open_success(){
    *args.res = file_add_open();
    if(args.success_proc)
        args.success_proc();
}

static void file_open_read_next(){
    if(str_cmp(args.name, args.fs->name) == 0){
        file_open_success();
        return;
    }
    if(args.fs->next){
        ata_read_sync(PRIMARY_BASE, 0, args.fs->next, 1, args.fs, file_open_read_next, file_open_error);
    }else{
        file_open_error();
    }
}


static void file_open_read_head(){
    if(args.parent->head){
        ata_read_sync(PRIMARY_BASE, 0, args.parent->head, 1, args.fs, file_open_read_next, file_open_error);
    }else{
        file_open_error();
    }
}

void file_open(dir_entity_t* parent, char* name, fs_entity_t** res, SUCC_ERR){
    args.res =  res;
    args.name = name;
    args.success_proc = success_proc;
    args.error_proc = error_proc;
    args.fs = (fs_entity_t*) alloc(SectorSize);
    if(!args.fs){
        if(error_proc){
            error_proc();
        }
        return;
    }
    
    if(!parent){
       if(!name){
            ata_read_sync(PRIMARY_BASE, 0, RootDirLBA, 1, args.fs, file_open_success, file_open_error);
        }else{
            if(error_proc)
                error_proc();
        }
        return;
    }

    if(!file_is_open((fs_entity_t*)parent) || parent->fs.type != DIR_TYPE){
        if(error_proc)
            error_proc();
        return;
    }

    args.parent = parent;
    file_open_read_head();
}
