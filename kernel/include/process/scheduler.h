#include "timer.h"

pcb_t* get_current_process();

void scheduler_init();

void scheduler_enable();

void schedule(cpu_state_t*);

void schedule_process(cpu_state_t*);

/* Excuted when the current process requests an unavailable resource*/
void schedule_waiting(cpu_state_t*);

void schedule_terminate_current_process(cpu_state_t*);

void schedule_terminate_current_thread(cpu_state_t*);

void schedule_thread(cpu_state_t*);

void context_switch(cpu_state_t* cpu, thread_t* next_thread);