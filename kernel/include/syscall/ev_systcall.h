#include "process.h"
#include "keyboard.h"
#include "mouse.h"
#include <stdint.h>

typedef struct{
    thread_t* thread;
    uint8_t is_waiting;
    uint32_t handler;
    void* args;
}event_t;

typedef struct{
    pcb_t* pcb;
    event_t mouse_handler;
    event_t keyboard_handler;
} pcb_event_handler_t;

typedef enum{
    MOUSE_EVENT = 1,
    KEYBOARD_EVENT = 2
} event_type_t;

void set_active_process(pcb_t*);

void register_event_handler(cpu_state_t*);

void deregister_event_handler(cpu_state_t*);

void wait_event_handler(cpu_state_t*);

void clear_events_handler(pcb_t*, thread_t*);

void ev_syscall_keyboard_handler(key_info_t);

void ev_syscall_mouse_handler(mouse_info_t);

void ev_syscall_init();