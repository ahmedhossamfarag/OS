#pragma once

#define FILE_TYPE 0x33
#define DIR_TYPE 0x55
#define NameLength 100
#define MAX_N_OPEN_FILES 20 
#include <stdint.h>
#include "memory.h"
#include "disk.h"
#include "ata.h"

typedef struct{
    uint8_t type;
    uint32_t lba;
    uint32_t parent;
    uint32_t prev;
    uint32_t next;
    char name[NameLength];
} __attribute__((packed)) fs_entity_t;

typedef struct{
    fs_entity_t fs;
    uint32_t data;
    uint32_t n_blocks;
    uint32_t size;
}  __attribute__((packed)) file_entity_t;

typedef struct{
    fs_entity_t fs;
    uint32_t head;
    uint32_t tail;
    uint32_t n_childs;
}  __attribute__((packed)) dir_entity_t;

void filesystem_init();

#define SUCC_ERR void (*success_proc)(), void (*error_proc)()
#define SUCC_ERR_V void (*success_proc)(); void (*error_proc)();

void dir_create(dir_entity_t* parent, char* name, fs_entity_t** res, SUCC_ERR);

void dir_append(dir_entity_t* parent, fs_entity_t* fs, SUCC_ERR);

void dir_remove(dir_entity_t* parent, fs_entity_t* fs, SUCC_ERR);

void dir_list(dir_entity_t* parent, char* res, SUCC_ERR);

void dir_delete(dir_entity_t* parent, SUCC_ERR);

void file_create(dir_entity_t* parent, char* name, fs_entity_t** res, SUCC_ERR);

void file_open(dir_entity_t* parent, char* name, fs_entity_t** res, SUCC_ERR);

void file_close(fs_entity_t* file, SUCC_ERR);

void file_read(file_entity_t* file, char* buffer, uint32_t sector_seek, uint32_t sector_count, SUCC_ERR);

void file_write(file_entity_t* file, char* buffer, uint32_t sector_count, uint32_t size, SUCC_ERR);

void file_delete(file_entity_t* file, SUCC_ERR);

uint8_t file_is_open(fs_entity_t* fs);