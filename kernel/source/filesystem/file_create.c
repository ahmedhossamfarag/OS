#include "file_system.h"
#include "strlib.h"

static struct{
    dir_entity_t* parent;
    file_entity_t* file;
    SUCC_ERR_V;
}args;

static void file_create_error(){
    disk_close(args.error_proc, args.error_proc);
}

static void file_create_close_disk(){
    disk_close(args.success_proc, args.error_proc);
}


static void file_create_find_lba(){
    file_entity_t* file = args.file;
    file->fs.lba = disk_alloc(1);
    if(!file->fs.lba){
        file_create_error();
        return;
    }
    dir_append(args.parent, (fs_entity_t*)file, file_create_close_disk, file_create_error);
}

void file_create(dir_entity_t* parent, char* name, fs_entity_t* res, SUCC_ERR){
    file_entity_t* file = (file_entity_t*) res;
    str_copy_n(name, file->fs.name, NameLength);
    file->fs.type = FILE_TYPE;
    file->n_blocks = file->size = file->data = 0;

    args.parent = parent;
    args.file = file;
    args.success_proc = success_proc;
    args.error_proc = error_proc;

    disk_open(file_create_find_lba, error_proc);
}