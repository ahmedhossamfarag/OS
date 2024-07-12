#include "scheduler.h"
#include "screen_print.h"
#include "strlib.h"

pcb_t processes[MAX_N_PROCESS];
uint8_t n_processes;

pcb_t* queue[MAX_N_PROCESS];
uint8_t queue_size;

pcb_t* current_process;

pcb_t* get_current_process(){
    return current_process;
}

uint8_t add_new_process(uint32_t pid, uint32_t ppid, uint32_t eip, uint32_t ebp){
    if(n_processes < MAX_N_PROCESS){
        processes[n_processes] = create_process(pid, ppid, eip, ebp);
        process_inqueue(&processes[n_processes]);
        n_processes++;
        return 1;
    }
    return 0;
}

void process_inqueue(pcb_t* process){
    if(queue_size < MAX_N_PROCESS){
        queue[queue_size] = process;
        queue_size++;
    }
}

pcb_t* process_dequeue(){

    if(queue_size > 0){
        pcb_t* next = queue[0];
        for (uint8_t i = 0; i < queue_size - 1; i++)
        {
            queue[i] = queue[i+1];
        }
        queue_size--;
        return next;
    }
    return 0;
}

void scheduler_init()
{
    n_processes = 0;
    queue_size = 0;
    current_process = 0;
}

void scheduler_enable()
{
    set_timer_handler_proc(schedule);
    enable_timer(10);
}

void schedule(cpu_state_t** state)
{
    pcb_t* next_process = process_dequeue();

    if(next_process){
        context_switch(state, next_process);

        if(current_process){
            current_process->process_state = PROCESS_STATE_READY;
            process_inqueue(current_process);   
        }

        next_process->process_state = PROCESS_STATE_RUNNING;
        current_process = next_process;
    }
}

void schedule_resource(cpu_state_t** state){
    pcb_t* next_process = process_dequeue();

    if(next_process){
        context_switch(state, next_process);

        if(current_process){
            current_process->process_state = PROCESS_STATE_WAITING;
        }

        next_process->process_state = PROCESS_STATE_RUNNING;
        current_process = next_process;
    }
}

void context_switch(cpu_state_t** cpu, pcb_t* next_process){

    if(current_process){
        current_process->state = *cpu;
    }
    *cpu = next_process->state;
}