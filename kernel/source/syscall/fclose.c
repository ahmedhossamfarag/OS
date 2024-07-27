#include "io_syscall.h"
#include "file_system.h"
#include "dslib.h"

extern array_t* open_files;

extern open_file_t* get_open_file(uint32_t pntr);

void fclose_handler(cpu_state_t* state)
{
    uint32_t pntr = state->eax;

    open_file_t* op = get_open_file(pntr);

    if(op){
        array_remove(open_files, (void**)pntr);
        free((char*)op->fs, SectorSize);
        free((char*)op, sizeof(open_file_t));
        state->eax = 1;
    }else{
        state->eax = 0;
    }
}