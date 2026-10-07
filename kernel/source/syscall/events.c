#include "ev_systcall.h"
#include "scheduler.h"
#include "libc.h"
#include "windows.h"
#include "vga_print.h"
#include "strlib.h"

pcb_event_handler_t* event_handlers;


static pcb_t* get_active_pcb(){
    window_t* active_window = get_active_window();
    if (active_window){
        return (pcb_t*) active_window->owner->parent;
    }
    return 0;
}

static pcb_t* get_running_pcb_at(int32_t x, int32_t y){
    window_t* window = get_window_at(x, y);
    if (window){
        return (pcb_t*) window->owner->parent;
    }
    return 0;
}

void register_event_queue(cpu_state_t* cpu){
    if(!cpu->rbx){
        cpu->rax = 0;
        return;
    }
    pcb_t* pcb = get_current_process();
    thread_t* thread = get_current_thread();
    uint8_t indx = get_process_index(pcb);
    pcb_event_handler_t* ev_handler = event_handlers + indx;
    event_queue_t* ev_queue = (event_queue_t*) cpu->rbx;
    ev_handler->queue = ev_queue;
    ev_handler->thread = thread;
    ev_handler->is_waiting = 0;
    cpu->rax = 1;
}

void deregister_event_queue(cpu_state_t* cpu){
    pcb_t* pcb = get_current_process();
    uint8_t indx = get_process_index(pcb);
    pcb_event_handler_t* ev_handler = event_handlers + indx;
    ev_handler->queue = 0;
    ev_handler->thread = 0;
    ev_handler->is_waiting = 0;
    cpu->rax = 1;
}

void clear_events_handler(pcb_t* pcb, thread_t* thread){
    uint8_t indx = get_process_index(pcb);
    pcb_event_handler_t* ev = event_handlers + indx;
    if (ev->thread == thread){
        ev->queue = 0;
        ev->thread = 0;
        ev->is_waiting = 0;
    }
}

void wait_event_handler(cpu_state_t* cpu){
    pcb_t* pcb = get_current_process();
    thread_t* thread = get_current_thread();
    uint8_t indx = get_process_index(pcb);
    pcb_event_handler_t* ev_handler = event_handlers + indx;
    ev_handler->thread = thread;
    ev_handler->is_waiting = 1;
    schedule_thread_waiting(cpu);
}

static uint64_t switch_to_process_cr3(pcb_t* pcb){
    uint64_t current_cr3;
    asm volatile("mov %%cr3, %0":"=r"(current_cr3));
    uint64_t process_cr3 = pcb->cr3;
    asm volatile("mov %0, %%cr3"::"r"(process_cr3));
    return current_cr3;
}

static void switch_to_current_cr3(uint64_t current_cr3){
    asm volatile("mov %0, %%cr3"::"r"(current_cr3));
}

static void ev_awake_thread(pcb_event_handler_t* ev){
    if(ev->is_waiting) {
        ev->is_waiting = 0;
        thread_awake(ev->thread);
    }
}

void ev_syscall_keyboard_handler(key_info_t k){
    pcb_t* active_pcb = get_active_pcb();
    if(!active_pcb){
        return;
    }
    pcb_t* pcb = active_pcb;
    uint8_t indx = get_process_index(pcb);
    pcb_event_handler_t* ev = event_handlers + indx;
    if(ev->queue){
        uint64_t current_cr3 = switch_to_process_cr3(active_pcb);
    
        event_queue_t* ev_queue = ev->queue;
        event_t* ev_event = ev_queue->events + ev_queue->write_index;
        ev_event->type = KEYBOARD_EVENT;
        ev_event->keyboard_info = k;
        int next_write_index = (ev_queue->write_index + 1) % ev_queue->capacity;
        if(next_write_index != ev_queue->read_index){
            ev_queue->write_index  = next_write_index;
        }

        switch_to_current_cr3(current_cr3);

        ev_awake_thread(ev);
    }
}

void ev_syscall_mouse_handler(mouse_info_t m){
    pcb_t* active_pcb = get_running_pcb_at(m.mouse_x, m.mouse_y);
    if(!active_pcb){
        return;
    }
    pcb_t* pcb = active_pcb;
    uint8_t indx = get_process_index(pcb);
    pcb_event_handler_t* ev = event_handlers + indx;
    if(ev->queue){
        uint64_t current_cr3 = switch_to_process_cr3(active_pcb);

        event_queue_t* ev_queue = ev->queue;
        event_t* ev_event = ev_queue->events + ev_queue->write_index;
        ev_event->type = MOUSE_EVENT;
        ev_event->mouse_info = m;
        int next_write_index = (ev_queue->write_index + 1) % ev_queue->capacity;
        if(next_write_index != ev_queue->read_index){
            ev_queue->write_index  = next_write_index;
        }

        switch_to_current_cr3(current_cr3);

        ev_awake_thread(ev);
    }
}