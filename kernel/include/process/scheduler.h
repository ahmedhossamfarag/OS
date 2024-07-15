#define MAX_N_PROCESS 10

#include "timer.h"

pcb_t* get_current_process();

uint8_t add_new_process(uint32_t pid, uint32_t ppid, uint32_t cr3, uint32_t ebp);

void process_inqueue(pcb_t* process);

pcb_t* process_dequeue();

void scheduler_init();

void scheduler_enable();

void schedule(cpu_state_t**);

/* Excuted when the current process requests an unavailable resource*/
void schedule_waiting(cpu_state_t**);

void context_switch(cpu_state_t* cpu, pcb_t* next_process);