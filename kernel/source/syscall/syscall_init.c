#include "io_syscall.h"
#include "ev_systcall.h"
#include "resources.h"
#include "memory.h"

extern resource_queue_t* keyboard_queue;
extern resource_queue_t* disk_queue;
extern pcb_event_handler_t* events_handlers;

void io_syscall_init(){
    keyboard_queue = resource_queue_new(KEYBOARD, QUEUE_CAPACITY);
    disk_queue = resource_queue_new(DISK, QUEUE_CAPACITY);
}

void ev_syscall_init(){
    events_handlers = (pcb_event_handler_t*) alloc(MAX_N_PROCESS*sizeof(pcb_event_handler_t));
    set_mouse_handler_proc(ev_syscall_mouse_handler);
    set_keyboard_handler_proc(ev_syscall_keyboard_handler);
}

void syscall_init(){
    io_syscall_init();
    ev_syscall_init();
}
