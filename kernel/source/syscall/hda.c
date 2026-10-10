#include "hda.h"
#include "resources.h"

void *hda_lock;

static struct {
    thread_t* thread;
} syscall_hda_args;

void awake_hda_waiting_thread(){
    thread_t* thread = syscall_hda_args.thread;
    if (thread) {
        thread_awake(thread);
        resource_lock_free(hda_lock, thread);
        syscall_hda_args.thread = 0;
    }
}

void play_sound_handler(cpu_state_t* state){
    thread_t* thread = get_current_thread();

    uint64_t* buffer = (uint64_t*) state->rdx;

    uint32_t size = state->rcx;

    uint64_t cr3 = ((pcb_t*)thread->parent)->cr3;

    resource_lock_request(hda_lock, thread);
    
    syscall_hda_args.thread = thread;

    if(hda_play_sound((void*)buffer, size, cr3, 0)){
        state->rax = 1;
    } else {
        state->rax = 0;
    }

    resource_lock_free(hda_lock, thread);
}

void record_sound_handler(cpu_state_t* state){
    thread_t* thread = get_current_thread();

    uint64_t* buffer = (uint64_t*) state->rdx;

    uint32_t size = state->rcx;

    uint64_t cr3 = ((pcb_t*)thread->parent)->cr3;

    resource_lock_request(hda_lock, thread);

    syscall_hda_args.thread = thread;

    if(hda_record_sound((void*)buffer, size, cr3, awake_hda_waiting_thread)){
        state->rax = 1;
        schedule_process_waiting(state);
    } else {
        state->rax = 0;
        resource_lock_free(hda_lock, thread);
    }
}

void clear_sound_handler(cpu_state_t*){
    thread_t* thread = get_current_thread();
    resource_lock_request(hda_lock, thread);
    hda_clear_args();
    resource_lock_free(hda_lock, thread);
}
