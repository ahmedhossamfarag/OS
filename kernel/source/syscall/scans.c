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

static void scans_keyboard_proc(key_info_t k){
    *args.des = k.character;
    args.nchars --;
    if(args.nchars){
        args.des ++;
    }else{
        scans_thread_awake();
    }
}

static void scans_line_keyboard_proc(key_info_t k){
    if(k.key_code != ENTER){
        *args.des = k.character;
        args.des ++;
    }else{
        *args.des = '\0';
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