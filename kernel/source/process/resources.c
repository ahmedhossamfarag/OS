/*

#include "resources.h"
#include "interrupt.h"
#include "strlib.h"
#include "memory.h"
#include "int_map.h"

resource_t* resources;


extern void isr_resource_request_handler();
extern void isr_resource_free_handler();

void resources_init(){
    resources = (resource_t*) alloc(RESOURCES_N * sizeof(resource_t));

    resource_t screen = {SCREEN, 0, 0};
    resources[0] = screen;
    
    idt_set_entry(RESOURCE_REQUEST_INT, (uint32_t)(isr_resource_request_handler));
    idt_set_entry(RESOURCE_FREE_INT, (uint32_t)(isr_resource_free_handler));
}

void resource_request_handler(cpu_state_t* state){
    asm("cli");

    uint32_t resource_id = state->eax;

    thread_t* thread = get_current_thread();

    for (resource_t* r = resources; r < resources + RESOURCES_N; r++)
    {
        if(r->id == resource_id){
            if(r->handler && r->handler != thread){
                if(r->n_waiters < MAX_N_PROCESS){
                    r->waiters[r->n_waiters] = thread;
                    r->n_waiters ++;
                }
                schedule_thread_waiting(state);
            }else{
                r->handler = thread;
            }
            break;
        }
    }
    
}

void resource_free_handler(){
    asm("cli");

    uint32_t resource_id;
    asm("mov %%eax, %0" : "=r"(resource_id));

    thread_t* thread = get_current_thread();

    for (resource_t* r = resources; r < resources + RESOURCES_N; r++)
    {
        if(r->id == resource_id){
            if(r->handler == thread){
                if(r->n_waiters > 0){
                    r->handler = r->waiters[0];
                    for (uint8_t i = 0; i < r->n_waiters - 1; i++)
                    {
                        r->waiters[i] = r->waiters[i+1];
                    }
                    
                    r->n_waiters --;
                    thread_awake(r->handler);
                }else{
                    r->handler = 0;
                }
            }
            break;
        }
    }
}
*/

#include "resources.h"
#include "memory.h"

resource_queue_t *resource_queue_new(resource_id_t id, uint32_t capacity)
{
    resource_queue_t* rq = (resource_queue_t*) alloc(sizeof(resource_queue_t));
    if(!rq){
        return 0;
    }
    rq->id = id;
    rq->handler = 0;
    rq->queue = queue_new(capacity, alloc);
    if(!rq->queue){
        return 0;
    }
    return rq;
}

void resource_queue_inque(resource_queue_t *rq, thread_t *thread, void (*proc)())
{
    if(!rq){
        return;
    }
    resource_lock_request(&rq->lock, thread);
    if(!rq->handler){
        rq->handler = thread;
        resource_lock_free(&rq->lock, thread);
        proc();
    }else{
        resource_request_t* req = (resource_request_t*) alloc(sizeof(resource_request_t));
        if(req){
            req->handler_proc = proc;
            req->thread = thread;
            queue_inque(rq->queue, req);
            resource_lock_free(&rq->lock, thread);
        }
        resource_lock_free(&rq->lock, thread);
    }
}

void resource_queue_deque(resource_queue_t *rq)
{
    if(!rq){
        return;
    }
    thread_t* thread = get_current_thread();
    resource_lock_request(&rq->lock, thread);
    rq->handler = 0;
    resource_request_t* next = queue_deque(rq->queue);
    if(next){
        rq->handler = next->thread;
        void (*proc)() = next->handler_proc;
        free((char*) next, sizeof(resource_request_t));
        resource_lock_free(&rq->lock, thread);
        proc();
    }else{
        resource_lock_free(&rq->lock, thread);
    }
}
