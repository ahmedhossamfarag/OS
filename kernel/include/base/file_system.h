#define FILE_TYPE 0x33
#define DIR_TYPE 0x55
#define NameLength 100
#define NamePrefixLength 5 
#define DirExEntity_N_Files 50
#define DirEntityIdentifier 0x88AA

#include <stdint.h>

typedef struct
{
    uint8_t type;
    uint32_t lba;
    char name_prefix[NamePrefixLength];
}__attribute__((packed)) File;

typedef struct
{
    uint32_t n_files;
    File* files;
} DirList;


typedef struct
{
    uint32_t lba;
    uint32_t n_sectors;
    uint32_t size;
    char name[NameLength];
   
}__attribute__((packed)) FileEntity;

typedef struct
{
    uint32_t identifier;
    uint32_t lba;
    uint8_t n_files;
    char name[NameLength];
}__attribute__((packed)) DirEntity;

typedef struct
{
    uint32_t next_lba;
    uint8_t n_files;
    File files[DirExEntity_N_Files];
}__attribute__((packed)) DirExEntity;


void file_system_init();

void file_get_info(File* file, FileEntity* entity);

void dir_get_info(File* dir, DirEntity* entity);

DirList dir_get_files_list(File* parent);

uint8_t file_read(File* file, uint32_t seek, uint32_t count, char* to);

uint8_t file_write(File* file, char* from, uint32_t size);

File file_create(File* parent, const char* name);

File dir_create(File* parent, const char* name);

uint8_t file_delete(File* parent, File* file);