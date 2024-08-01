#include "file_system.h"
#include "ata.h"
#include "memory.h"
#include "disk.h"
#include "dslib.h"

extern array_t* open_files;

void filesystem_init(){
    open_files = array_new(MAX_N_OPEN_FILES, alloc);
    dir_entity_t* root = (dir_entity_t*) alloc(SectorSize);
    if(root){
        ata_read(PRIMARY_BASE, 0, RootDirLBA, 1, root);
        if(root->fs.type != DIR_TYPE){
            root->fs.lba = RootDirLBA;
            root->fs.parent = root->fs.prev = root->fs.next = root->fs.name[0] = 0;
            root->fs.type = DIR_TYPE;
            root->head = root->tail = 0;
            root->n_childs = 0;
            ata_write(PRIMARY_BASE, 0, RootDirLBA, 1, root);
        }
    }
}


uint8_t file_is_open(fs_entity_t* fs){
    for (void** f = open_files->data; f < open_files->data + open_files->size; f++)
    {
        if(((fs_entity_t*) *f) == fs){
            return 1;
        }
    }
    return 0;
}