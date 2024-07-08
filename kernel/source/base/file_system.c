#include "file_system.h"
#include "disk.h"
#include "memory.h"
#include "ata.h"
#include "strlib.h"
#include "math.h"

static uint32_t create_dir_ex(File* first_file){
    uint32_t lba = 0;
    DirExEntity* dir = (DirExEntity*) alloc(SectorSize);
    if(dir){
        lba = disk_alloc(1);
        if(lba)
        {
            dir->next_lba = 0;
            dir->n_files = 0;
            File file = {0,0};
            for (File* f = dir->files; f < dir->files+ DirExEntity_N_Files; f++)
            {
                *f = file;
            }
            if(first_file){
                dir->files[0] = *first_file;
                dir->n_files = 1;
            }
            ata_write(PRIMARY_BASE, 0, lba, 1, dir);
        }
        free((char*)dir, SectorSize);
    }
    return lba;
}

static uint8_t dir_append(uint32_t lba, File* file){
    if(!lba) return 0;

    DirExEntity* dir = (DirExEntity*) alloc(SectorSize);
    uint8_t done = 0;
    if(dir){
        do
        {
            ata_read(PRIMARY_BASE, 0, lba, 1, dir);
            if(dir->n_files < DirExEntity_N_Files){
                dir->files[dir->n_files] = *file;
                dir->n_files++;
                ata_write(PRIMARY_BASE, 0, lba, 1, dir);
                done = 1;
                break;
            }
            if(done) break;
            lba = dir->next_lba;
        } while (lba);
        
        if(!done){
            dir->next_lba = create_dir_ex(file);
            if(dir->next_lba){
                ata_write(PRIMARY_BASE, 0, lba, 1, dir);
                done = 1;
            };
        }
        free((char*)dir, SectorSize);
    }
    return done;
}

static void dir_list(uint32_t lba, File* files){
    if(!lba) return;

    DirExEntity* dir = (DirExEntity*) alloc(SectorSize);

    if(dir){
        do
        {
            ata_read(PRIMARY_BASE, 0, lba, 1, dir);
            for (File*  f = dir->files; f < dir->files + dir->n_files; f++)
            {
                *files = *f;
                files++;
            }
            lba = dir->next_lba;
        } while (lba);
        free((char*)dir, SectorSize);
    }
}

static File dir_find(uint32_t lba, const char* name){
    DirExEntity* dir = (DirExEntity*) alloc(SectorSize);
    File file = {0,0};

    if(!lba) return file;

    if(dir){
        char* entity = alloc(SectorSize);
        char* entity_name;
        if(entity){
            do
            {
                ata_read(PRIMARY_BASE, 0, lba, 1, dir);
                for (File*  f = dir->files; f < dir->files + dir->n_files; f++)
                {
                    if(str_start_with(name, f->name_prefix)){
                        ata_read(PRIMARY_BASE, 0, f->lba, 1, entity);
                        if(f->type == FILE_TYPE)
                            entity_name = ((FileEntity*)entity)->name;
                        else
                            entity_name = ((DirEntity*)entity)->name;
                        if(str_cmp(name, entity_name) == 0){
                            file = *f;
                            break;
                        }
                    }
                }
                if(file.lba) break;
                lba = dir->next_lba;
            } while (lba);
            free((char*)entity, SectorSize);
        }
        free((char*)dir, SectorSize);
    }
    return file;
}

static void file_delete_helper(char* entity, File* f);

static uint8_t dir_remove(uint32_t lba, File* file){
    if(!lba) return 0;

    DirExEntity* dir = (DirExEntity*) alloc(SectorSize);
    uint8_t done = 0;
    if(dir){
        char* entity =  alloc(SectorSize);
        if(entity){
            do
            {
                ata_read(PRIMARY_BASE, 0, lba, 1, dir);
                for (File*  f = dir->files; f < dir->files + dir->n_files; f++)
                {
                    if(f->lba == file->lba){
                        file_delete_helper(entity, f);
                        for (File*  f2 = dir->files; f2 < dir->files + dir->n_files - 1; f2++)
                        {
                            *f2 = *(f2+1);
                        }
                        dir->n_files --;
                        ata_write(PRIMARY_BASE, 0, lba, 1, dir);
                        done = 1;
                        break;
                    }
                }
                if(done) break;
                lba = dir->next_lba;
            } while (lba);
            free((char*)entity, SectorSize);
        }
        free((char*)dir, SectorSize);
    }
    return done;
}

static uint8_t dir_delete(uint32_t lba){
    if(!lba) return 0;

    DirExEntity* dir = (DirExEntity*) alloc(SectorSize);
    uint8_t done = 0;
    if(dir){
        char* entity = alloc(SectorSize);
        if(entity){
            do
            {
                ata_read(PRIMARY_BASE, 0, lba, 1, dir);
                for (File*  f = dir->files; f < dir->files + dir->n_files; f++)
                {
                    file_delete_helper(entity, f);
                }
                disk_free(lba, 1);
                lba = dir->next_lba;
            } while (lba);
            done = 1;
            free((char*)entity, SectorSize);
        }
        free((char*)dir, SectorSize);
    }
    return done;
}

static void file_delete_helper(char* entity, File* f){
    ata_read(PRIMARY_BASE, 0, f->lba, 1, entity);
    if(f->type == FILE_TYPE){
        if(((FileEntity*)entity)->lba)
            disk_free(((FileEntity*)entity)->lba, ((FileEntity*)entity)->n_sectors);
    }else if(f->type == DIR_TYPE){
        if(((DirEntity*)entity)->lba)
            dir_delete(((DirEntity*)entity)->lba);
    }
    disk_free(f->lba, 1);
}


void file_system_init()
{
    DirEntity* dir = (DirEntity*) alloc(SectorSize);
    if(dir){
        ata_read(PRIMARY_BASE, 0, RootDirLBA, 1, dir);
        if(dir->identifier != DirEntityIdentifier){
            dir->identifier = DirEntityIdentifier;
            dir->lba = create_dir_ex(NULL);
            dir->n_files = 0;
            ata_write(PRIMARY_BASE, 0, RootDirLBA, 1, dir);
        }
        free((char*)dir, SectorSize);
    }   
}

void file_get_info(File *file, FileEntity *entity)
{
    if(!file || !file->lba || !entity) return;
    ata_read(PRIMARY_BASE, 0, file->lba, 1, entity);
}

void dir_get_info(File *dir, DirEntity *entity)
{
    if(!dir || !dir->lba || !entity) return;
    ata_read(PRIMARY_BASE, 0, dir->lba, 1, entity);
}

DirList dir_get_files_list(File *parent)
{
    DirList list;
    uint32_t lba = parent ? parent->lba : RootDirLBA;
    DirEntity* dir = (DirEntity*) alloc(SectorSize);
    if(dir){
        ata_read(PRIMARY_BASE, 0, lba, 1, dir);
        list.n_files = dir->n_files;
        if(dir->lba){
            list.files = (File*) alloc(sizeof(File)*list.n_files);
            if(list.files){
                dir_list(dir->lba, list.files);
            }
        }
        free((char*)dir, SectorSize);
    }
    return list;
}

uint8_t file_read(File* file, uint32_t seek, uint32_t count, char* to){
    if(!file || !file->lba || file->type != FILE_TYPE || count == 0 || !to) return 0;

    FileEntity* entity = (FileEntity*) alloc(SectorSize);
    uint8_t done = 0;
    if(entity){
        ata_read(PRIMARY_BASE, 0, file->lba, 1, entity);
        if(seek + count <= entity->n_sectors){
            ata_read(PRIMARY_BASE, 0, entity->lba+seek, count, to);
            done = 1;
        }
        free((char*)entity, SectorSize);
    }
    return done;
}

uint8_t file_write(File* file, char* from, uint32_t size){
    if(!file || !file->lba || file->type != FILE_TYPE || !from || size == 0) return 0;

    uint32_t n_sectors = math_ciel( (float) size / SectorSize);
    FileEntity* entity = (FileEntity*) alloc(SectorSize);
    uint8_t done = 0;
    if(entity){
        ata_read(PRIMARY_BASE, 0, file->lba, 1, entity);
        if(entity->lba){
            disk_free(entity->lba, entity->n_sectors);
        }
        entity->lba = disk_alloc(n_sectors);
        if(entity->lba){
            ata_write(PRIMARY_BASE, 0, entity->lba, n_sectors, from);
            entity->size = size;
            entity->n_sectors = n_sectors;
            ata_write(PRIMARY_BASE, 0, file->lba, 1, entity);
            done = 1;
        }
        free((char*)entity, SectorSize);
    }
    return done;
}

File file_create(File* parent, const char* name){
    File file = {FILE_TYPE, 0};

    if(!name) return file;

    FileEntity* entity = (FileEntity*) alloc(SectorSize);
    if(entity){
        file.lba = disk_alloc(1);
        if(file.lba){
            entity->lba = 0;
            entity->n_sectors = 0;
            entity->size = 0;
            str_copy(name, entity->name);
            ata_write(PRIMARY_BASE, 0, file.lba, 1, entity);
            str_copy_n(name, file.name_prefix, NamePrefixLength);
            uint32_t lba = parent ? parent->lba : RootDirLBA;
            DirEntity* dir_entity = (DirEntity*) entity;
            ata_read(PRIMARY_BASE, 0, lba, 1, dir_entity);
            if (!dir_entity->lba) {
                dir_entity->lba = create_dir_ex(&file);
                if (dir_entity->lba) {
                    dir_entity->n_files++;
                    ata_write(PRIMARY_BASE, 0, lba, 1, dir_entity);
                }
            }else if(dir_append(dir_entity->lba, &file)){
                dir_entity->n_files ++;
                ata_write(PRIMARY_BASE, 0, lba, 1, dir_entity);
            }
        }
        free((char*)entity, SectorSize);
    }
    return file;
}

File dir_create(File* parent, const char* name){
    File file = {DIR_TYPE, 0};

    if(!name) return file;

    DirEntity* entity = (DirEntity*) alloc(SectorSize);
    if(entity){
        file.lba = disk_alloc(1);
        if(file.lba){
            entity->identifier = DirEntityIdentifier;
            entity->lba = create_dir_ex(NULL);
            entity->n_files = 0;
            str_copy(name, entity->name);
            ata_write(PRIMARY_BASE, 0, file.lba, 1, entity);
            str_copy_n(name, file.name_prefix, NamePrefixLength);
            uint32_t lba = parent ? parent->lba : RootDirLBA;
            DirEntity* dir_entity = (DirEntity*) entity;
            ata_read(PRIMARY_BASE, 0, lba, 1, dir_entity);
            if (!dir_entity->lba) {
                dir_entity->lba = create_dir_ex(&file);
                if (dir_entity->lba) {
                    dir_entity->n_files++;
                    ata_write(PRIMARY_BASE, 0, lba, 1, dir_entity);
                }
            }else if(dir_append(dir_entity->lba, &file)){
                dir_entity->n_files ++;
                ata_write(PRIMARY_BASE, 0, lba, 1, dir_entity);
            }
        }
        free((char*)entity, SectorSize);
    }
    return file;
}

uint8_t file_delete(File* parent, File* file){
    if(!file || !file->lba) return 0;

    if(parent){
        if(parent->type != DIR_TYPE) return 0;
    }
    uint8_t done = 0;
    DirEntity* dir = (DirEntity*) alloc(SectorSize);
    if(dir){
        uint32_t lba = parent ? parent->lba : RootDirLBA;
        ata_read(PRIMARY_BASE, 0, lba, 1, dir);
        if(dir->lba){
            done = dir_remove(dir->lba, file);
            if(done){
                dir->n_files --;
                ata_write(PRIMARY_BASE, 0, lba, 1, dir);
            }
        }
        free((char*)dir, SectorSize);
    }
    return done;
}