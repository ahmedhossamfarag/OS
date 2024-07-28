#include "th_syscall.h"
#include "info.h"
#include "apic.h"
#include "int_map.h"
#include "pagging.h"

void thread_create_handler(cpu_state_t* state){
    state->eax = add_new_thread(get_current_process(), state->eax, state->ebx, state->edx);
}

void process_exit_handler(cpu_state_t* state){
    if(info_get_processor_id() == 0){
        schedule_process_terminated(state);
    }else{
        apic_send_ipi(0, PROCESS_TERMINATED_INT);
    }
}

void thread_exit_handler(cpu_state_t* state){
    schedule_thread_terminated(state);
}

void process_terminate_handler(cpu_state_t* state){
    pcb_t* curr_prcss = get_current_process();
    pcb_t* prcss = get_process_pid(curr_prcss->pid, state->eax);
    if(!prcss){
        state->eax = 0;
        return;
    }
    remove_process(prcss);
    state->eax = 1;
}

void thread_terminate_handler(cpu_state_t* state){
    pcb_t* curr_prcss = get_current_process();
    thread_t* thr = get_thread_tid(curr_prcss, state->eax);
    if(!thr){
        state->eax = 0;
        return;
    }
    apic_send_ipi(thr->processor_id, THREAD_TERMINATED_INT);
    state->eax = 1;
}

void memory_init_handler(cpu_state_t* state)
{
    pcb_t* pcb = get_current_process();
    state->ebx = pcb->memo_begin;
    state->edx = PROCESS_N_PAGE_TABLES * PAGE_SIZE;
}
