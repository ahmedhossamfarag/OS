#define SCHEDULE_INT 0x40

#include "timer.h"

pcb_t* get_current_process();

thread_t* get_current_thread();

void scheduler_init();

void enable_scheduler();

void schedule(cpu_state_t*);

void schedule_process(cpu_state_t*);

/* Excuted when the current process requests an unavailable resource*/
void schedule_waiting(cpu_state_t*);

void schedule_terminate_current_process(cpu_state_t*);

void schedule_terminate_current_thread(cpu_state_t*);

void schedule_thread(cpu_state_t*);

void context_switch(cpu_state_t* cpu, thread_t* current_thread, thread_t* next_thread, uint32_t cr3);