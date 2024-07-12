#include "process.h"
#include "memory.h"

pcb_t create_process(uint32_t pid, uint32_t ppid, uint32_t eip, uint32_t ebp){
    pcb_t pcb;
    pcb.pid = pid;
    pcb.ppid = ppid;

    pcb.process_state = PROCESS_STATE_READY;

    pcb.state = (cpu_state_t*)(ebp - sizeof(cpu_state_t));
    pcb.state->cs = 0x8;
    pcb.state->eip = eip;
    pcb.state->eflags = EFLAGS_DEFAULT;
    pcb.state->ds = USER_DS;
    pcb.state->es = USER_DS;
    pcb.state->gs = USER_DS;
    pcb.state->fs = USER_DS;
    pcb.state->ebp = ebp;
    pcb.state->esp= ebp - 12;
    
    return pcb;
}
