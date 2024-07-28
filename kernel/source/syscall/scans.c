#include "io_syscall.h"
#include "resources.h"
#include "keyboard.h"

resource_queue_t* keyboard_queue;

static struct{
    char* des;
    uint32_t nchars;
} args;

static void scans_thread_awake(){
    set_keyboard_handler_proc(0);
    keyboard_queue->handler->cpu_state.eax = 1;
    thread_awake(keyboard_queue->handler);
    resource_queue_deque(keyboard_queue);
}

static void scans_write_char(char c){
    uint32_t current_cr3;
    asm("mov %%cr3, %0":"=r"(current_cr3));
    uint32_t th_cr3 = ((pcb_t*)keyboard_queue->handler->parent)->cr3;
    asm volatile("mov %0, %%cr3" :: "r"(th_cr3));

    *args.des = c;

    asm volatile("mov %0, %%cr3" :: "r"(current_cr3));
}

static void scans_keyboard_proc(key_info_t k){
    scans_write_char(k.character);
    args.nchars --;
    if(args.nchars){
        args.des ++;
    }else{
        scans_thread_awake();
    }
}

static void scans_line_keyboard_proc(key_info_t k){
    if(k.key_code != ENTER){
        scans_write_char(k.character);
        args.des ++;
    }else{
        scans_write_char('\0');
        scans_thread_awake();
    }
}

static void scans_proc(){
    cpu_state_t* state = &keyboard_queue->handler->cpu_state;

    args.des = (char*) state->edx;
    args.nchars = state->ecx;

    set_keyboard_handler_proc(scans_keyboard_proc);
}

void scans_handler(cpu_state_t* state)
{
    uint32_t n = state->ecx;

    if(n == 0){
        state->eax = 1;
        return;
    }   

    if(keyboard_queue->queue->size == keyboard_queue->queue->capacity){
        state->eax = 0;
        return;
    }

    schedule_process_waiting(state);

    resource_queue_inque(keyboard_queue, get_current_thread(), scans_proc);
    
}

static void scans_line_proc(){
    cpu_state_t* state = &keyboard_queue->handler->cpu_state;

    args.des = (char*) state->edx;

    set_keyboard_handler_proc(scans_line_keyboard_proc);

}

void scans_line_handler(cpu_state_t* state)
{
    uint32_t n = state->ecx;

    if(n == 0){
        state->eax = 1;
        return;
    }

    if(keyboard_queue->queue->size == keyboard_queue->queue->capacity){
        state->eax = 0;
        return;
    }

    schedule_process_waiting(state);

    resource_queue_inque(keyboard_queue, get_current_thread(), scans_line_proc);
}