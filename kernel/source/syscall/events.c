#include "ev_systcall.h"
#include "scheduler.h"
#include "libc.h"
#include "vga_print.h"
#include "strlib.h"

pcb_event_handler_t* events_handlers;

static pcb_t* active_pcb = 0;

void set_active_process(pcb_t* pcb){
    active_pcb = pcb;
}

void register_event_handler(cpu_state_t* cpu){
    if(!cpu->ebx || !cpu->edx){
        cpu->eax = 0;
        return;
    }
    pcb_t* pcb = get_current_process();
    uint8_t indx = get_process_index(pcb);
    pcb_event_handler_t* ev = events_handlers + indx;
    ev->pcb = pcb;
    thread_t* thread = get_current_thread();
    switch (cpu->eax)
    {
    case MOUSE_EVENT:
        ev->mouse_handler.handler = cpu->ebx;
        ev->mouse_handler.args = (void*) cpu->edx;
        ev->mouse_handler.thread = thread;
        ev->mouse_handler.is_waiting = 0;
        break;
    case KEYBOARD_EVENT:
        ev->keyboard_handler.handler = cpu->ebx;
        ev->keyboard_handler.args = (void*) cpu->edx;
        ev->keyboard_handler.thread = thread;
        ev->keyboard_handler.is_waiting = 0;
    default:
        break;
    }

    cpu->eax = 1;
}

void deregister_event_handler(cpu_state_t* cpu){
    pcb_t* pcb = get_current_process();
    uint8_t indx = get_process_index(pcb);
    pcb_event_handler_t* ev = events_handlers + indx;
    ev->pcb = pcb;
    switch (cpu->eax)
    {
    case MOUSE_EVENT:
        ev->mouse_handler.handler = 0;
        ev->mouse_handler.args = 0;
        ev->mouse_handler.is_waiting = 0;
        break;
    case KEYBOARD_EVENT:
        ev->keyboard_handler.handler = 0;
        ev->keyboard_handler.args = 0;
        ev->keyboard_handler.is_waiting = 0;
    default:
        break;
    }
    cpu->eax = 1;
}

static void ev_wait_thread(pcb_t* pcb, thread_t* thread){
    uint8_t indx = get_process_index(pcb);
    pcb_event_handler_t* ev = events_handlers + indx;
    
    if(ev->mouse_handler.thread == thread){
        ev->mouse_handler.is_waiting = 1;
    }
    if(ev->keyboard_handler.thread == thread){
        ev->keyboard_handler.is_waiting = 1;
    }
}

void wait_event_handler(cpu_state_t* cpu){
    ev_wait_thread(get_current_process(), get_current_thread());
    schedule_thread_waiting(cpu);
}

void clear_events_handler(pcb_t* pcb, thread_t* thread){
    uint8_t indx = get_process_index(pcb);
    pcb_event_handler_t* ev = events_handlers + indx;
    if(ev->mouse_handler.thread == thread){
        ev->mouse_handler.args = 0;
        ev->mouse_handler.handler = 0;
        ev->mouse_handler.is_waiting = 0;
    }
    if(ev->keyboard_handler.thread == thread){
        ev->keyboard_handler.args = 0;
        ev->keyboard_handler.handler = 0;
        ev->keyboard_handler.is_waiting = 0;
    }
}

static void ev_copy_args(pcb_t* pcb, void* to, void* args, uint32_t size){
    uint32_t current_cr3;
    asm("mov %%cr3, %0":"=r"(current_cr3));
    uint32_t th_cr3 = pcb->cr3;
    asm volatile("mov %0, %%cr3" :: "r"(th_cr3));

    mem_copy((char*)args, (char*)to, size);

    asm volatile("mov %0, %%cr3" :: "r"(current_cr3));
}

static void ev_push_eip(uint32_t cr3, cpu_state_t* cpu){
    uint32_t current_cr3;
    asm("mov %%cr3, %0":"=r"(current_cr3));
    uint32_t th_cr3 = cr3;
    asm volatile("mov %0, %%cr3" :: "r"(th_cr3));

    uint32_t* esp = (uint32_t*)cpu->user_esp;
    esp --;
    *esp = cpu->eip;
    cpu->user_esp = (uint32_t)esp;

    asm volatile("mov %0, %%cr3" :: "r"(current_cr3));
}

static void ev_awake_handler(uint32_t cr3, event_t event){
    if(event.thread->thread_state == THREAD_STATE_WAITING){
        ev_push_eip(cr3, &event.thread->cpu_state);
        event.thread->cpu_state.eip = event.handler;
        thread_awake(event.thread);
    }
}

static void ev_awake_thread(pcb_event_handler_t* ev, thread_t* thread){
    if(ev->mouse_handler.thread == thread){
        ev->mouse_handler.is_waiting = 0;
    }
    if(ev->keyboard_handler.thread == thread){
        ev->keyboard_handler.is_waiting = 0;
    }
}

void ev_syscall_keyboard_handler(key_info_t k){
    if(!active_pcb){
        return;
    }
    pcb_t* pcb = active_pcb;
    uint8_t indx = get_process_index(pcb);
    pcb_event_handler_t* ev = events_handlers + indx;
    if(ev->keyboard_handler.args && ev->keyboard_handler.handler){
        if(ev->keyboard_handler.is_waiting){
            ev_copy_args(pcb, ev->keyboard_handler.args, &k, sizeof(key_info_t));
            ev_awake_handler(pcb->cr3, ev->keyboard_handler);
            ev_awake_thread(ev, ev->keyboard_handler.thread);
        }
    }
}

void ev_syscall_mouse_handler(mouse_info_t m){
    if(!active_pcb){
        return;
    }
    pcb_t* pcb = active_pcb;
    uint8_t indx = get_process_index(pcb);
    pcb_event_handler_t* ev = events_handlers + indx;
    if(ev->mouse_handler.args && ev->mouse_handler.handler){
        if(ev->mouse_handler.is_waiting){
            ev_copy_args(pcb, ev->mouse_handler.args, &m, sizeof(mouse_info_t));
            ev_awake_handler(pcb->cr3, ev->mouse_handler);
            ev_awake_thread(ev, ev->mouse_handler.thread);
        }
    }
}