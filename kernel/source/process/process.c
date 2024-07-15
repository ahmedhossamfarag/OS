#include "process.h"
#include "memory.h"

pcb_t create_process(uint32_t pid, uint32_t ppid, uint32_t cr3, uint32_t ebp){
    pcb_t pcb;
    pcb.pid = pid;
    pcb.ppid = ppid;

    pcb.process_state = PROCESS_STATE_READY;

    pcb.cpu_state.eip = 0;
    pcb.cpu_state.cs = USER_CS;
    pcb.cpu_state.eflags = EFLAGS_DEFAULT;
    pcb.cpu_state.user_esp= ebp;
    pcb.cpu_state.user_ss = USER_DS;
    pcb.cpu_state.ds = USER_DS;
    pcb.cpu_state.es = USER_DS;
    pcb.cpu_state.gs = USER_DS;
    pcb.cpu_state.fs = USER_DS;
    pcb.cpu_state.ebp = ebp;

    pcb.cr3 = cr3;

    return pcb;
}
