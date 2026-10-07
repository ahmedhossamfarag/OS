#include "process.h"
#include "keyboard.h"
#include "mouse.h"
#include <stdint.h>

typedef enum{
    MOUSE_EVENT = 1,
    KEYBOARD_EVENT = 2
} event_type_t;

typedef struct{
    event_type_t type;
    union{
        mouse_info_t mouse_info;
        key_info_t keyboard_info;
    };
} event_t;

typedef struct{
    event_t* events;
    int capacity;
    int read_index;
    int write_index;
} event_queue_t;

typedef struct {
    thread_t* thread;
    uint8_t is_waiting;
    event_queue_t* queue;
} pcb_event_handler_t;

void register_event_queue(cpu_state_t*);

void deregister_event_queue(cpu_state_t*);

void clear_events_handler(pcb_t*, thread_t*);

void wait_event_handler(cpu_state_t*);

void ev_syscall_keyboard_handler(key_info_t);

void ev_syscall_mouse_handler(mouse_info_t);

void ev_syscall_init();