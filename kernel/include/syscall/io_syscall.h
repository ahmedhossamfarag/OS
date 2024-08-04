#include "process.h"
#include "file_system.h"
#include <stdint.h>

#define QUEUE_CAPACITY 10

typedef struct
{
    fs_entity_t* fs;
    thread_t* handler;
} open_file_t;


void fopen_handler(cpu_state_t*);

void fsize_handler(cpu_state_t*);

void fclose_handler(cpu_state_t*);

void fread_handler(cpu_state_t*);

void fwrite_handler(cpu_state_t*);

void fcreate_handler(cpu_state_t*);

void fdelete_handler(cpu_state_t*);

void flist_handler(cpu_state_t*);

void prints_handler(cpu_state_t*);

void scans_handler(cpu_state_t*);

void io_syscall_init();