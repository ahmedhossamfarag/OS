#include "file_system.h"
#include "dslib.h"

extern array_t* open_files;

fs_entity_t** file_get_open(fs_entity_t* fs){
    for (void** f = open_files->data; f < open_files->data + open_files->size; f++)
    {
        if(((fs_entity_t*) *f)->lba == fs->lba){
            return (fs_entity_t**) f;
        }
    }
    return 0;
}

fs_entity_t** file_get_open_lba(uint32_t lba){
    for (void** f = open_files->data; f < open_files->data + open_files->size; f++)
    {
        if(((fs_entity_t*) *f)->lba == lba){
            return (fs_entity_t**) f;
        }
    }
    return 0;
}

void file_close(fs_entity_t* file, SUCC_ERR){
    fs_entity_t** fs_pntr = file_get_open(file);
    if(fs_pntr){
        array_remove(open_files, (void**)fs_pntr);
        free((char*) *fs_pntr, SectorSize);
        if(success_proc){
            success_proc();
        }
    }else{
        if(error_proc){
            error_proc();
        }
    }
}