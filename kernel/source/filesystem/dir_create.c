#include "file_system.h"
#include "strlib.h"
#include "dslib.h"

extern array_t* open_files;

static struct{
    dir_entity_t* parent;
    dir_entity_t* dir;
    fs_entity_t** res;
    SUCC_ERR_V;
}args;

static void dir_create_error(){
    disk_close(args.error_proc, args.error_proc);
}

static void dir_create_success(){
    array_add(open_files, args.dir);
    *args.res = (fs_entity_t*)args.dir;
    if(args.success_proc)
        args.success_proc();
}

static void dir_create_close_disk(){
    disk_close(dir_create_success, args.error_proc);
}


static void dir_create_find_lba(){
    dir_entity_t* dir = args.dir;
    dir->fs.lba = disk_alloc(1);
    if(!dir->fs.lba){
        dir_create_error();
        return;
    }
    dir_append(args.parent, (fs_entity_t*)dir, dir_create_close_disk, dir_create_error);
}

void dir_create(dir_entity_t* parent, char* name, fs_entity_t** res, SUCC_ERR){
    if(!parent || !name || !*name || !file_is_open((fs_entity_t*)parent)){
        if(error_proc){
            error_proc();
        }
        return;
    }

    dir_entity_t* dir = (dir_entity_t*) alloc(SectorSize);
    if(!dir){
        if(error_proc)
            error_proc();
        return;
    }
    str_copy_n(name, dir->fs.name, NameLength);
    dir->fs.type = DIR_TYPE;
    dir->head = dir->tail = dir->n_childs = 0;

    args.parent = parent;
    args.dir = dir;
    args.res = res;
    args.success_proc = success_proc;
    args.error_proc = error_proc;

    disk_open(dir_create_find_lba, error_proc);
}