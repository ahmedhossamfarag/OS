#include "win_syscall.h"
#include "resources.h"
#include "windows.h"

void* windows_lock;

void register_window_handler(cpu_state_t* state){    
    thread_t* thread = get_current_thread();

    window_t* window = (window_t*)state->rdx;

    resource_lock_request(&windows_lock, thread);

    window_t* success = register_window(window);

    resource_lock_free(&windows_lock, thread); 

    state->rax = (uint64_t)success;
}

void deregister_window_handler(cpu_state_t* state){
    thread_t* thread = get_current_thread();

    window_t* window = (window_t*)state->rdx;

    resource_lock_request(&windows_lock, thread);

    uint8_t success = unregister_window(window);

    resource_lock_free(&windows_lock, thread);

    state->rax = success;
}

void redraw_window_handler(cpu_state_t* state){
    thread_t* thread = get_current_thread();
    
    resource_lock_request(&windows_lock, thread);

    window_t* window = (window_t*)state->rdx;
    redraw_window(window);

    resource_lock_free(&windows_lock, thread);

    state->rax = 1;
}

void update_window_bounds_handler(cpu_state_t* state){
    thread_t* thread = get_current_thread();
    
    resource_lock_request(&windows_lock, thread);

    window_t* window = (window_t*)state->rdx;
    bounds_t* bounds = (bounds_t*)state->rcx;
    update_window_bounds(window, bounds);

    resource_lock_free(&windows_lock, thread);

    state->rax = 1;
}