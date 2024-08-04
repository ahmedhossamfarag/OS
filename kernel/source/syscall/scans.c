#include "io_syscall.h"
#include "resources.h"
#include "vga_edit.h"
#include "libc.h"

resource_queue_t* keyboard_queue;

static struct{
    char* des;
    uint32_t nchars;
} args;

static void scans_thread_awake(){
    thread_awake(keyboard_queue->handler);
    resource_queue_deque(keyboard_queue);
}

static void scans_write_str(char* str){
    uint32_t current_cr3;
    asm("mov %%cr3, %0":"=r"(current_cr3));
    uint32_t th_cr3 = ((pcb_t*)keyboard_queue->handler->parent)->cr3;
    asm volatile("mov %0, %%cr3" :: "r"(th_cr3));

    mem_copy(str, args.des, args.nchars);

    asm volatile("mov %0, %%cr3" :: "r"(current_cr3));
}

static void scans_end(char* str){
    if(str){
        scans_write_str(str);
        keyboard_queue->handler->cpu_state.eax = 1;
    }else{
        keyboard_queue->handler->cpu_state.eax = 0;
    }
    scans_thread_awake();
}

static void scans_proc(){
    cpu_state_t* state = &keyboard_queue->handler->cpu_state;

    args.des = (char*) state->edx;
    args.nchars = state->ecx;

    vga_edit_init(args.nchars, scans_end);
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

    thread_t* thread = get_current_thread();

    schedule_process_waiting(state);

    resource_queue_inque(keyboard_queue, thread, scans_proc);
    
}