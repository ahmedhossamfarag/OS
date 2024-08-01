#include "file_system.h"

void file_read(file_entity_t* file, char* buffer, uint32_t sector_seek, uint32_t sector_count, SUCC_ERR){
    if(!file_is_open((fs_entity_t*)file) || file->fs.type != FILE_TYPE){
        if(error_proc)
            error_proc();
        return;
    }
    if(file->n_blocks >= sector_seek){
        if(error_proc)
            error_proc();
        return;
    }
    if(sector_count + sector_seek > file->n_blocks){
        sector_count = file->n_blocks - sector_seek;
    }
    if(!sector_count){
        if(success_proc)
            success_proc();
        return;
    }
    ata_read_sync(PRIMARY_BASE, 0, file->data + sector_seek, sector_count, buffer, success_proc, error_proc);
}