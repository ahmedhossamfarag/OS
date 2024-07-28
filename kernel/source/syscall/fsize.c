#include "io_syscall.h"
#include "file_system.h"
#include "dslib.h"

extern array_t* open_files;

open_file_t* get_open_file(uint32_t pntr){
    return (open_file_t*) array_get(open_files, (void**) pntr);
}

void fsize_handler(cpu_state_t* state)
{
    uint32_t pntr = state->eax;
    uint32_t *to = (uint32_t*) state->edx;

    open_file_t* op = get_open_file(pntr);

    if(op && op->fs->type == FILE_TYPE){
        state->eax = 1;
        *to = ((file_entity_t*)op->fs)->size;
    }else if(op && op->fs->type == DIR_TYPE){
        state->eax = 1;
        *to = ((dir_entity_t*)op->fs)->n_childs;
    }else{
        state->eax = 0;
    }
}
