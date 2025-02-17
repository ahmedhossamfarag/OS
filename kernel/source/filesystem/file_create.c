#include "file_system.h"
#include "strlib.h"
#include "dslib.h"

extern array_t* open_files;

static struct{
    dir_entity_t* parent;
    file_entity_t* file;
    fs_entity_t** res;
    SUCC_ERR_V;
}args;

static void file_create_error(){
    disk_close(args.error_proc, args.error_proc);
}

static void file_create_success(){
    array_add(open_files, args.file);
    *args.res = (fs_entity_t*)args.file;
    if(args.success_proc)
        args.success_proc();
}

static void file_create_close_disk(){
    disk_close(file_create_success, args.error_proc);
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

void file_create(dir_entity_t* parent, char* name, fs_entity_t** res, SUCC_ERR){
    if(!parent || !name || !*name || !file_is_open((fs_entity_t*)parent)){
        if(error_proc){
            error_proc();
        }
        return;
    }

    file_entity_t* file = (file_entity_t*) alloc(SectorSize);
    if(!file){
        if(error_proc)
            error_proc();
        return;
    }
    str_copy_n(name, file->fs.name, NameLength);
    file->fs.type = FILE_TYPE;
    file->n_blocks = file->size = file->data = 0;

    args.parent = parent;
    args.file = file;
    args.res = res;
    args.success_proc = success_proc;
    args.error_proc = error_proc;

    disk_open(file_create_find_lba, error_proc);
}