#include "io_syscall.h"
#include "file_system.h"
#include "dslib.h"

static struct{
    cpu_state_t* state;
} args;

static void fclose_error(){
    args.state->eax = 0;
}

static void fclose_success(){
    args.state->eax = 1;
}

void fclose_handler(cpu_state_t* state)
{
    uint32_t fs = state->eax;
    if(fs){
        args.state = state;
        file_close((fs_entity_t*)fs, fclose_success, fclose_error);
    }else{
        state->eax = 0;
    }
}