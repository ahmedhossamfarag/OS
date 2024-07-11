#include "process.h"
#include "memory.h"

pcb_t create_process(uint32_t pid, uint32_t ppid, uint32_t eip, uint32_t ebp){
    pcb_t pcb;
    pcb.pid = pid;
    pcb.ppid = ppid;
    pcb.registers.cs = USER_CS;
    pcb.registers.eip = eip;
    pcb.registers.ds = USER_DS;
    pcb.registers.es = USER_DS;
    pcb.registers.fs = USER_DS;
    pcb.registers.gs = USER_DS;
    pcb.registers.eax = 0;
    pcb.registers.ebx = 0;
    pcb.registers.ecx = 0;
    pcb.registers.edx = 0;
    pcb.registers.esi = 0;
    pcb.registers.edi = 0;
    pcb.registers.eflags = EFLAGS_DEFAULT;
    pcb.process_state = PROCESS_STATE_READY;
    return pcb;
}
