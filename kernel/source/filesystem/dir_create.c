#include "file_system.h"
#include "strlib.h"

static struct{
    dir_entity_t* parent;
    dir_entity_t* dir;
    SUCC_ERR_V;
}args;

static void dir_create_error(){
    disk_close(args.error_proc, args.error_proc);
}

static void dir_create_close_disk(){
    disk_close(args.success_proc, args.error_proc);
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

void dir_create(dir_entity_t* parent, char* name, fs_entity_t* res, SUCC_ERR){
    dir_entity_t* dir = (dir_entity_t*) res;
    str_copy_n(name, dir->fs.name, NameLength);
    dir->fs.type = DIR_TYPE;
    dir->head = dir->tail = dir->n_childs = 0;

    args.parent = parent;
    args.dir = dir;
    args.success_proc = success_proc;
    args.error_proc = error_proc;

    disk_open(dir_create_find_lba, error_proc);
}