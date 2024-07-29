#include "process.h"

void interrupt_handler_init();

void exception_handler(cpu_state_t* cpu);

void pic_handler(void);

void syscall_handler(cpu_state_t* state);