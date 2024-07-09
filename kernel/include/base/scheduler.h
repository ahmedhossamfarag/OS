#include "process.h"

void add_process(pcb_t* process);

pcb_t* remove_process();

void schudle();

void context_switch(pcb_t* next_process);