#include "resources.h"
#include "interrupt.h"
#include "screen_print.h"
#include "strlib.h"
resource_t resources[RESOURCES_N];


extern void isr_resource_request_handler();
extern void isr_resource_free_handler();

void resources_init(){
    resource_t screen = {SCREEN, 0, 0};
    resources[0] = screen;
    
    set_idt_entry(RESOURCE_REQUEST_INT, (uint32_t)(isr_resource_request_handler));
    set_idt_entry(RESOURCE_FREE_INT, (uint32_t)(isr_resource_free_handler));
}

void resource_request_handler(cpu_state_t* state){
    asm("cli");

    uint32_t resource_id = state->eax;

    pcb_t* process = get_current_process();

    for (resource_t* r = resources; r < resources + RESOURCES_N; r++)
    {
        if(r->id == resource_id){
            if(r->handler && r->handler != process){
                if(r->n_waiters < MAX_N_PROCESS){
                    r->waiters[r->n_waiters] = process;
                    r->n_waiters ++;
                }
                schedule_resource(state);
            }else{
                r->handler = process;
            }
            break;
        }
    }
    
}

void resource_free_handler(){
    asm("cli");

    uint32_t resource_id;
    asm("mov %%eax, %0" : "=r"(resource_id));

    pcb_t* process = get_current_process();

    for (resource_t* r = resources; r < resources + RESOURCES_N; r++)
    {
        if(r->id == resource_id){
            if(r->handler == process){
                if(r->n_waiters > 0){
                    r->handler = r->waiters[0];
                    for (uint8_t i = 0; i < r->n_waiters - 1; i++)
                    {
                        r->waiters[i] = r->waiters[i+1];
                    }
                    
                    r->n_waiters --;
                    r->handler->process_state = PROCESS_STATE_READY;
                    process_inqueue(r->handler);
                }else{
                    r->handler = 0;
                }
            }
            break;
        }
    }
}