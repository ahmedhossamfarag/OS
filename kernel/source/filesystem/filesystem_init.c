#include "file_system.h"
#include "ata.h"
#include "memory.h"
#include "disk.h"

void filesystem_init(){
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