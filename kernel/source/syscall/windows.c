#include "win_syscall.h"
#include "resources.h"
#include "windows.h"

void* windows_lock;

void register_window_handler(cpu_state_t* state){    
    thread_t* thread = get_current_thread();

    window_t* window = (window_t*)state->edx;

    resource_lock_request(&windows_lock, thread);

    window_t* success = register_window(window);

    resource_lock_free(&windows_lock, thread); 

    state->eax = (uint32_t)success;
}

void deregister_window_handler(cpu_state_t* state){
    thread_t* thread = get_current_thread();

    window_t* window = (window_t*)state->edx;

    resource_lock_request(&windows_lock, thread);

    uint8_t success = unregister_window(window);

    resource_lock_free(&windows_lock, thread);

    state->eax = success;
}

void redraw_window_handler(cpu_state_t* state){
    thread_t* thread = get_current_thread();
    
    resource_lock_request(&windows_lock, thread);

    window_t* window = (window_t*)state->edx;
    redraw_window(window);

    resource_lock_free(&windows_lock, thread);

    state->eax = 1;
}

void update_window_bounds_handler(cpu_state_t* state){
    thread_t* thread = get_current_thread();
    
    resource_lock_request(&windows_lock, thread);

    window_t* window = (window_t*)state->edx;
    bounds_t* bounds = (bounds_t*)state->ecx;
    update_window_bounds(window, bounds);

    resource_lock_free(&windows_lock, thread);

    state->eax = 1;
}