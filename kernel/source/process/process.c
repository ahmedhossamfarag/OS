#include "process.h"
#include "memory.h"
#include "pagging.h"
#include "info.h"
#include "dslib.h"
#include "resources.h"
#include "screen_print.h"
#include "strlib.h"
#include "ev_systcall.h"

pcb_t* default_process;

pcb_t* processes;

queue_t** queues;
void** queues_lock;

void* pstate_lock;

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
    uint8_t n_processors = info_get_processor_no();
    queues = (queue_t**) alloc(n_processors*sizeof(queue_t*));
    for (uint8_t i = 0; i < n_processors; i++)
    {
        queues[i] = queue_new(MAX_N_PROCESS*MAX_N_THREAD, alloc);
    }

    queues_lock = (void**) alloc(n_processors*sizeof(void*));
    for (uint8_t i = 0; i < n_processors; i++)
    {
        queues_lock[i] = 0;
    }

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

    pstate_lock = 0;
}

pcb_t* get_default_process(){
    return default_process;
}

static uint8_t next_processor_id;
static uint8_t get_next_processor_id(){
    uint8_t res = next_processor_id;
    next_processor_id = (next_processor_id + 1) % info_get_processor_no();
    return res;
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

    thread->processor_id = get_next_processor_id();

}

static void create_process(pcb_t* pcb, uint32_t pid, uint32_t ppid, uint32_t cr3, uint32_t eip, uint32_t ebp, uint32_t memo_begin){
    pcb->pid = pid;
    pcb->ppid = ppid;
    pcb->process_state = PROCESS_STATE_READY;
    pcb->cr3 = cr3;
    pcb->memo_begin = memo_begin;

    for (thread_t* t = pcb->threads; t < pcb->threads + MAX_N_THREAD; t++)
    {
            t->thread_state = THREAD_STATE_TERMINATED;
    }

    thread_t* main_thread = pcb->threads;
    create_thread(main_thread, 0, eip, ebp);
    pcb->n_active_threads = 1;
}

uint8_t add_new_process(uint32_t pid, uint32_t ppid, uint32_t cr3, uint32_t eip, uint32_t ebp, uint32_t memo_begin){
    resource_lock_request(&pstate_lock, (void*)(info_get_processor_id()+1));

    for (pcb_t* p = processes; p < processes + MAX_N_PROCESS; p++)
    {
        if(p->n_active_threads){
            if(p->pid == pid){
                return 0;
            }
        }
    }

    uint8_t res = 0;
    for (pcb_t* p = processes; p < processes + MAX_N_PROCESS; p++)
    {
        if(p->process_state == PROCESS_STATE_TERMINATED && !p->n_active_threads){
            create_process(p, pid, ppid, cr3, eip, ebp, memo_begin);
            thread_inqueue(p->threads);
            res = 1;
            break;
        }
    }

    resource_lock_free(&pstate_lock, (void*)(info_get_processor_id()+1));

    return res;
}

static void process_free_thread(pcb_t* process){
    process->n_active_threads --;
    if(!process->n_active_threads){
        process->process_state = PROCESS_STATE_TERMINATED;
        free_pagging_dir((uint32_t*)process->cr3);
    }
}

void remove_process(pcb_t *process)
{
    resource_lock_request(&pstate_lock, (void*)(info_get_processor_id()+1));
    process->process_state = PROCESS_STATE_TERMINATED;
    for (thread_t* t = process->threads; t < process->threads + MAX_N_THREAD; t++)
    {
        if(t->thread_state != THREAD_STATE_RUNNING){
            if(t->thread_state == THREAD_STATE_READY){
                thread_remove(t);
            }
        }
    }
    resource_lock_free(&pstate_lock, (void*)(info_get_processor_id()+1));
    
}

pcb_t* get_process_pid(uint32_t ppid, uint32_t pid)
{
    pcb_t* process = 0;
    for (pcb_t* p = processes; p < processes  + MAX_N_PROCESS; p++)
    {
        if(p->process_state != PROCESS_STATE_TERMINATED && p->pid == pid && p->ppid == ppid){
            process = p;
            break;
        }
    }
    return process;
}

void thread_inqueue(thread_t* thread){
    uint8_t n = thread->processor_id;
    resource_lock_request(queues_lock + n, (void*)(n+1));
    queue_inque(queues[n], thread);
    thread->thread_state = THREAD_STATE_READY;
    resource_lock_free(queues_lock + n, (void*)(n+1));
}

thread_t* thread_dequeue(){    
    uint8_t n = info_get_processor_id();
    resource_lock_request(queues_lock + n, (void*)(n+1));
    thread_t* thread = (thread_t*) queue_deque(queues[n]);
    resource_lock_free(queues_lock + n, (void*)(n+1));
    return thread;
}

void thread_remove(thread_t* thread){
    uint8_t n = thread->processor_id;
    resource_lock_request(queues_lock + n, (void*)(n+1));
    if(queue_remove(queues[n], thread)){
        thread->thread_state = THREAD_STATE_TERMINATED;
        process_free_thread((pcb_t*)thread->parent);
        clear_events_handler((pcb_t*)thread->parent, thread);
    }
    resource_lock_free(queues_lock + n, (void*)(n+1));
}

uint8_t add_new_thread(pcb_t *process, uint32_t tid, uint32_t eip, uint32_t ebp)
{
    resource_lock_request(&pstate_lock, (void*)(info_get_processor_id()+1));

    for (uint8_t i = 0; i < MAX_N_THREAD; i++)
    {
        thread_t* t = process->threads + i;
        if(t->thread_state != THREAD_STATE_TERMINATED){
            if(t->tid == tid){
                return 0;
            }
        }
    }

    uint8_t res = 0;
    for (uint8_t i = 0; i < MAX_N_THREAD; i++)
    {
        thread_t* t = process->threads + i;
        if(t->thread_state == THREAD_STATE_TERMINATED){
            create_thread(t, tid, eip, ebp);
            process->n_active_threads ++;
            thread_inqueue(t);
            res = 1;
            break;
        }
    }
    resource_lock_free(&pstate_lock, (void*)(info_get_processor_id()+1));
    
    return res;
}

void remove_thread(thread_t* thread)
{
    resource_lock_request(&pstate_lock, (void*)(info_get_processor_id()+1));
    if(thread->thread_state == THREAD_STATE_RUNNING){
        thread->thread_state = THREAD_STATE_TERMINATED;
        pcb_t* pcb = (pcb_t*)thread->parent;
        process_free_thread(pcb);
        clear_events_handler(pcb, thread);
    }
    resource_lock_free(&pstate_lock, (void*)(info_get_processor_id()+1));
}

thread_t* get_thread_tid(pcb_t *process, uint32_t tid)
{
    for (thread_t* t = process->threads; t < process->threads + MAX_N_THREAD; t++)
    {
        if(t->thread_state != THREAD_STATE_TERMINATED && t->tid == tid){
            return t;
        }
    }
    return 0;
}

thread_t* get_process_thread(pcb_t *process, uint8_t n)
{
    if(n < MAX_N_THREAD){
        thread_t* t = process->threads + n;
        return t;
    }
    return 0;
}

void thread_waiting(thread_t* thread){
    resource_lock_request(&pstate_lock, (void*)(info_get_processor_id()+1));
    if(thread->thread_state == THREAD_STATE_RUNNING){
        thread->thread_state = THREAD_STATE_WAITING;
    }    
    resource_lock_free(&pstate_lock, (void*)(info_get_processor_id()+1));
}

void thread_awake(thread_t *thread)
{
    resource_lock_request(&pstate_lock, (void*)(info_get_processor_id()+1));

    if(thread->thread_state == THREAD_STATE_WAITING){
        pcb_t* pcb = (pcb_t*)thread->parent;
        if(pcb->process_state == PROCESS_STATE_TERMINATED){
            thread->thread_state = THREAD_STATE_TERMINATED;
            process_free_thread(pcb);
        }else{
            if(thread->parent != default_process){
            thread_inqueue(thread);
            }else{
                thread->thread_state = THREAD_STATE_READY;
            }
        }
    }

    resource_lock_free(&pstate_lock, (void*)(info_get_processor_id()+1));
}

uint8_t get_process_index(pcb_t *pcb)
{
    return pcb - processes;
}
