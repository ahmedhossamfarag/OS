#include "io_syscall.h"
#include "resources.h"
#include "vga_print.h"

void* prints_lock;

void prints_handler(cpu_state_t* state)
{
    thread_t* thread = get_current_thread();

    char* str = (char*)state->eax;

    resource_lock_request(&prints_lock, thread);

    print(str);

    resource_lock_free(&prints_lock, thread); 

    state->eax = 1;   
}

