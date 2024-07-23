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
}__attribute__((packed)) file_t;

typedef struct
{
    uint32_t n_files;
    file_t* files;
} dir_list_t;


typedef struct
{
    uint32_t lba;
    uint32_t n_sectors;
    uint32_t size;
    char name[NameLength];
   
}__attribute__((packed)) file_entity_t;

typedef struct
{
    uint32_t identifier;
    uint32_t lba;
    uint8_t n_files;
    char name[NameLength];
}__attribute__((packed)) dir_entity_t;

typedef struct
{
    uint32_t next_lba;
    uint8_t n_files;
    file_t files[DirExEntity_N_Files];
}__attribute__((packed)) dir_ex_entity_t;


void file_system_init();

void file_get_info(file_t* file, file_entity_t* entity);

void dir_get_info(file_t* dir, dir_entity_t* entity);

dir_list_t dir_get_files_list(file_t* parent);

file_t file_open(file_t* parent, const char* name);

uint8_t file_read(file_t* file, uint32_t seek, uint32_t count, char* to);

uint8_t file_write(file_t* file, char* from, uint32_t size);

file_t file_create(file_t* parent, const char* name);

file_t dir_create(file_t* parent, const char* name);

uint8_t file_delete(file_t* parent, file_t* file);