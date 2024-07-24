#include "process.h"
#include "memory.h"
#include "pagging.h"
#include "info.h"
#include "dslib.h"

pcb_t* default_process;

pcb_t* processes;

queue_t* queue;

static void default_process_init(){
    default_process = (pcb_t*) alloc(sizeof(pcb_t));

    default_process->cr3 = (uint32_t) get_default_pagging_dir();
    default_process->process_state = PROCESS_STATE_RUNNING;
    default_process->n_active_threads = MAX_N_THREAD;
    for (uint8_t i = 0; i < MAX_N_THREAD; i++)
    {
        thread_t* t = default_process->threads + i;
        t->processor_id = i;
        t->thread_state = THREAD_STATE_RUNNING;
        t->parent = default_process;
    }
}

void process_init(){
    processes = (pcb_t*) alloc(MAX_N_PROCESS * sizeof(pcb_t));

    queue = queue_new(MAX_N_PROCESS, alloc);

    for (pcb_t* p = processes; p < processes + MAX_N_PROCESS; p++)
    {
        p->process_state = PROCESS_STATE_TERMINATED;
        p->n_active_threads = 0;
        for (thread_t* t = p->threads; t < p->threads + MAX_N_THREAD; t++)
        {
            t->parent = p;
            t->thread_state = THREAD_STATE_TERMINATED;
        }
    }

    default_process_init();
}

pcb_t* get_default_process(){
    return default_process;
}

static void create_thread(thread_t* thread, uint32_t tid, uint32_t eip, uint32_t ebp){
    thread->tid = tid;

    thread->thread_state = THREAD_STATE_READY;
    
    thread->cpu_state.eip = eip;
    thread->cpu_state.cs = USER_CS;
    thread->cpu_state.eflags = EFLAGS_DEFAULT;
    thread->cpu_state.user_esp= ebp;
    thread->cpu_state.user_ss = USER_DS;
    thread->cpu_state.ds = USER_DS;
    thread->cpu_state.es = USER_DS;
    thread->cpu_state.gs = USER_DS;
    thread->cpu_state.fs = USER_DS;
    thread->cpu_state.ebp = ebp;

}

static void create_process(pcb_t* pcb, uint32_t pid, uint32_t ppid, uint32_t cr3, uint32_t ebp){
    pcb->pid = pid;
    pcb->ppid = ppid;
    pcb->process_state = PROCESS_STATE_READY;
    pcb->cr3 = cr3;

    for (thread_t* t = pcb->threads; t < pcb->threads + MAX_N_THREAD; t++)
    {
            t->thread_state = THREAD_STATE_TERMINATED;
    }

    add_new_thread(pcb, 0, 0, ebp);
}

uint8_t add_new_process(uint32_t pid, uint32_t ppid, uint32_t cr3, uint32_t ebp){
    for (pcb_t* p = processes; p < processes + MAX_N_PROCESS; p++)
    {
        if(p->process_state == PROCESS_STATE_TERMINATED){
            create_process(p, pid, ppid, cr3, ebp);
            process_inqueue(p);
            return 1;
        }
    }
    

    return 0;
}

void remove_process(pcb_t *process)
{
    for (pcb_t* p = processes; p < processes  + MAX_N_PROCESS; p++)
    {
        if(p == process){
            p->process_state = PROCESS_STATE_TERMINATED;
        }
    }
    queue_remove(queue, process);
    
}

void process_inqueue(pcb_t* process){
    queue_inque(queue, process);
}

pcb_t* process_dequeue(){

    return (pcb_t*) queue_deque(queue);
}

uint8_t add_new_thread(pcb_t *process, uint32_t tid, uint32_t eip, uint32_t ebp)
{
    for (uint8_t i = 0; i < MAX_N_THREAD; i++)
    {
        thread_t* t = process->threads + i;
        if(t->thread_state == THREAD_STATE_TERMINATED){
            create_thread(t, tid, eip, ebp);
            process->n_active_threads ++;
            t->processor_id = i;
            return 1;
        }
    }
    
    
    return 0;
}

void remove_thread(pcb_t *process, thread_t* thread)
{
    for (thread_t* t = process->threads; t < process->threads + MAX_N_THREAD; t++)
    {
        if(t == thread){
            t->thread_state = THREAD_STATE_TERMINATED;
            process->n_active_threads--;
            break;
        }
    }
}

thread_t* get_process_thread(pcb_t *process, uint8_t n)
{
    if(n < MAX_N_THREAD){
        thread_t* t = process->threads + n;
        return t;
    }
    return 0;
}

void process_awake(pcb_t *process)
{
    if(process->process_state == PROCESS_STATE_WAITING){
        process->process_state = PROCESS_STATE_READY;
        process->n_active_threads ++;
        if(process != default_process){
            process_inqueue(process);
        }
    }
}

void thread_awake(thread_t *thread)
{
    if(thread->thread_state == THREAD_STATE_WAITING){
        thread->thread_state = THREAD_STATE_READY;
        process_awake((pcb_t*)thread->parent);
    }
}
