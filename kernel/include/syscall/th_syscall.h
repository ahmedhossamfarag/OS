#include "scheduler.h"
#include <stdint.h>


void process_create_handler(cpu_state_t* state);

void thread_create_handler(cpu_state_t* state);

void process_exit_handler(cpu_state_t* state);

void thread_exit_handler(cpu_state_t* state);

void process_terminate_handler(cpu_state_t* state);

void thread_terminate_handler(cpu_state_t* state);

void memory_init_handler(cpu_state_t* state);