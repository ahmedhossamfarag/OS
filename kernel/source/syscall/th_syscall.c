#include "th_syscall.h"
#include "info.h"
#include "apic.h"
#include "int_map.h"
#include "paging.h"

void thread_create_handler(cpu_state_t* state){
    state->rax = add_new_thread(get_current_process(), state->rax, state->rbx, state->rdx);
}

void process_exit_handler(cpu_state_t* state){
    schedule_process_terminated(state);
}

void thread_exit_handler(cpu_state_t* state){
    schedule_thread_terminated(state);
}

void process_terminate_handler(cpu_state_t* state){
    pcb_t* curr_prcss = get_current_process();
    pcb_t* prcss = get_process_pid(curr_prcss->pid, state->rax);
    if(!prcss){
        state->rax = 0;
        return;
    }
    remove_process(prcss);
    state->rax = 1;
}

void thread_terminate_handler(cpu_state_t* state){
    pcb_t* curr_prcss = get_current_process();
    thread_t* thr = get_thread_tid(curr_prcss, state->rax);
    if(!thr || thr == get_current_thread()){
        state->rax = 0;
        return;
    }
    apic_send_ipi(thr->processor_id, THREAD_TERMINATED_INT);
    state->rax = 1;
}

void memory_init_handler(cpu_state_t* state)
{
    pcb_t* pcb = get_current_process();
    state->rbx = pcb->memo_begin;
    state->rdx = PROCESS_N_PD_ENTRIES * PAGE_SIZE * NUM_PAGES;
}
