#include "process.h"
#include "memory.h"
#include "pagging.h"

pcb_t default_process;

pcb_t* processes;
uint8_t n_processes;

pcb_t** queue;
uint8_t queue_size;

void process_init(){
    processes = (pcb_t*) alloc(MAX_N_PROCESS * sizeof(pcb_t));
    queue = (pcb_t**) alloc(MAX_N_PROCESS * sizeof(pcb_t*));

    n_processes = 0;
    queue_size = 0;

    default_process.cr3 = (uint32_t) get_default_pagging_dir();
    default_process.process_state = PROCESS_STATE_RUNNING;
    default_process.n_threads = 1;
}

pcb_t* get_default_process(){
    return &default_process;
}

thread_t create_thread(uint32_t tid, uint32_t eip, uint32_t ebp){
    thread_t thread;
    thread.tid = tid;
    
    thread.cpu_state.eip = eip;
    thread.cpu_state.cs = USER_CS;
    thread.cpu_state.eflags = EFLAGS_DEFAULT;
    thread.cpu_state.user_esp= ebp;
    thread.cpu_state.user_ss = USER_DS;
    thread.cpu_state.ds = USER_DS;
    thread.cpu_state.es = USER_DS;
    thread.cpu_state.gs = USER_DS;
    thread.cpu_state.fs = USER_DS;
    thread.cpu_state.ebp = ebp;

    return thread;
}


pcb_t create_process(uint32_t pid, uint32_t ppid, uint32_t cr3, uint32_t ebp){
    pcb_t pcb;
    pcb.pid = pid;
    pcb.ppid = ppid;
    pcb.process_state = PROCESS_STATE_READY;
    pcb.cr3 = cr3;
    pcb.n_threads = 0;
    add_new_thread(&pcb, 0, 0, ebp);

    return pcb;
}


uint8_t add_new_process(uint32_t pid, uint32_t ppid, uint32_t cr3, uint32_t ebp){
    if(n_processes < MAX_N_PROCESS){
        processes[n_processes] = create_process(pid, ppid, cr3, ebp);
        process_inqueue(&processes[n_processes]);
        n_processes++;
        return 1;
    }
    return 0;
}

void remove_process(pcb_t *process)
{
    for (pcb_t* p = processes; p < processes + n_processes; p++)
    {
        if(p == process){
            for (pcb_t* p_ = p; p_ < processes + n_processes - 1; p_++)
            {
                *p_ = *(p_+1);
            }
            n_processes--;
            break;
        }
    }
    for (int j = 0; j < queue_size; j++)
    {
        if(queue[j] == process){
            for (uint8_t i = j; i < queue_size - 1; i++)
            {
                queue[i] = queue[i+1];
            }
            queue_size--;
            break;
        }
    }
    
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

uint8_t add_new_thread(pcb_t *process, uint32_t tid, uint32_t eip, uint32_t ebp)
{
    if(process->n_threads < MAX_N_THREAD){
        process->threads[process->n_threads] = create_thread(tid, eip, ebp);
        process->n_threads++;
        return 1;
    }
    return 0;
}

void remove_thread(pcb_t *process, uint32_t tid)
{
    for (thread_t* t = process->threads; t < process->threads + process->n_threads; t++)
    {
        if(t->tid == tid){
            for (thread_t* t_ = t; t_ < process->threads + process->n_threads -1; t_++)
            {
                *t_ = *(t_+1);
            }
            process->n_threads--;
            break;
        }
    }
    if(process->n_threads == 0){
        remove_process(process);
    }
}

thread_t* get_process_thread(pcb_t *process, uint8_t n)
{
    return &process->threads[n%process->n_threads];
}
