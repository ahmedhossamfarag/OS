#include "scheduler.h"

uint8_t thread_n;

pcb_t* current_process;
thread_t* current_thread;

pcb_t* get_current_process(){
    return current_process;
}

void scheduler_init()
{
    process_init();
    thread_n = 0;
    current_process = get_default_process();
    current_thread = get_process_thread(current_process, 0);
}

void scheduler_enable()
{
    set_timer_handler_proc(schedule);
    enable_timer(10);
}

void schedule(cpu_state_t* state)
{
    if(thread_n == 0){
        schedule_process(state);
    }else{
        schedule_thread(state);
    }
    thread_n = (thread_n + 1) % MAX_N_THREAD;
}

void schedule_process(cpu_state_t* state){
    pcb_t* next_process = process_dequeue();

    if(next_process){
        if(current_process){
            current_process->process_state = PROCESS_STATE_READY;
            process_inqueue(current_process);   
        }

        next_process->process_state = PROCESS_STATE_RUNNING;
        current_process = next_process;

    }
    schedule_thread(state);
}

void schedule_thread(cpu_state_t* state){
    thread_t* next_thread = get_process_thread(current_process, thread_n);
    if(current_thread != next_thread){
        context_switch(state, next_thread);
        current_thread = next_thread;
    }
}

void schedule_waiting(cpu_state_t* state){
    pcb_t* next_process = process_dequeue();
    
    if(!next_process){
        next_process = get_default_process();
    }

    if(next_process){
        if(current_process){
            current_process->process_state = PROCESS_STATE_WAITING;
        }

        next_process->process_state = PROCESS_STATE_RUNNING;
        current_process = next_process;

    }
    schedule_thread(state);
}

void schedule_terminate_current_process(cpu_state_t* state){
    pcb_t* next_process = process_dequeue();

    if(!next_process){
        next_process = get_default_process();
    }

    if(next_process){
        if(current_process){
            current_process->process_state = PROCESS_STATE_TERMINATED;
            remove_process(current_process);
        }

        next_process->process_state = PROCESS_STATE_RUNNING;
        current_process = next_process;

    }
    schedule_thread(state);
}

void schedule_terminate_current_thread(cpu_state_t* state){
    remove_thread(current_process, current_thread->tid);
    if(current_process->n_threads){
        schedule_thread(state);
    }else{
        schedule_terminate_current_process(state);
    }
}

void context_switch(cpu_state_t* cpu, thread_t* next_thread){

    if(current_thread){
        current_thread->cpu_state = *cpu;
    }
    next_thread->cpu_state.esp = cpu->esp;
    *cpu = next_thread->cpu_state;
    asm volatile("mov %0, %%cr3" :: "r"(current_process->cr3));
}