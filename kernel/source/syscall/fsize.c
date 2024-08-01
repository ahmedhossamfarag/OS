#include "io_syscall.h"
#include "file_system.h"
#include "dslib.h"

void fsize_handler(cpu_state_t* state)
{
    fs_entity_t* fs = (fs_entity_t*) state->eax;
    uint32_t *to = (uint32_t*) state->edx;

    if(file_is_open(fs)){
        if(fs->type == FILE_TYPE){
            state->eax = 1;
            *to = ((file_entity_t*)fs)->size;
        }else if(fs->type == DIR_TYPE){
            state->eax = 1;
            *to = ((dir_entity_t*)fs)->n_childs;
        }else{
            state->eax = 0;
        }
    }else{
        state->eax = 0;
    }
}
