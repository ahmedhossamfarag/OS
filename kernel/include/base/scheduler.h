#include "timer.h"

void add_process(pcb_t* process);

pcb_t* remove_process();

void schudler_init();

void schudle(cpu_state_t*);

void context_switch(pcb_t* next_process);