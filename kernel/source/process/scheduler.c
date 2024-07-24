#include "scheduler.h"
#include "info.h"
#include "apic.h"
#include "interrupt.h"
#include "int_map.h"

extern pcb_t* default_process;

pcb_t* current_process;
pcb_t* next_process;

pcb_t* get_current_process(){
    return current_process;
}

thread_t *get_current_thread()
{
    return get_process_thread(current_process, info_get_processor_id());
}

extern void isr_schedule_thread();
extern void isr_schedule_process_waiting();
extern void isr_schedule_thread_waiting();
extern void isr_schedule_process_terminated();
extern void isr_schedule_thread_terminated();

void scheduler_init()
{
    process_init();
    current_process = default_process;
    next_process = 0;

    idt_set_entry(SCHEDULE_INT, (uint32_t) isr_schedule_thread);
    idt_set_entry(PROCESS_WAITING_INT, (uint32_t)isr_schedule_process_waiting);
    idt_set_entry(THREAD_WAITING_INT, (uint32_t)isr_schedule_thread_waiting);
    idt_set_entry(PROCESS_TERMINATED_INT, (uint32_t)isr_schedule_process_terminated);
    idt_set_entry(THREAD_TERMINATED_INT, (uint32_t)isr_schedule_thread_terminated);
}

void enable_scheduler()
{
    if(info_get_processor_id() == 0){
        set_timer_handler_proc(schedule);
        enable_timer(10);
        set_lapic_timer_handler_proc(schedule);
        enable_lapic_timer(0x1000, LAPIC_TIMER_PERIODIC_MODE);
    }
}

void schedule(cpu_state_t* state)
{
    schedule_process(state);   
}

static void schedule_send_int(){
    for (uint8_t apic_id = 1; apic_id < info_get_processor_no(); apic_id++)
    {
        apic_send_ipi(apic_id, SCHEDULE_INT);
    }
}

void schedule_process(cpu_state_t* state){
    next_process = process_dequeue();

    if(next_process){
        if(current_process){
            if(current_process != default_process){
                if(current_process->n_active_threads){
                    process_inqueue(current_process);
                }else{
                    current_process->process_state = PROCESS_STATE_WAITING;
                }
            }  
        }

        schedule_send_int();

        schedule_thread(state);

        current_process = next_process;
        next_process->process_state = PROCESS_STATE_RUNNING;

    }
}

void schedule_thread(cpu_state_t* state){
    if(next_process){
        uint32_t cpu_id = info_get_processor_id();

        thread_t* current_thread = get_process_thread(current_process, cpu_id);
        thread_t* next_thread = get_process_thread(next_process, cpu_id);

        if(!next_thread || next_thread->thread_state != THREAD_STATE_READY){
            next_thread = get_process_thread(get_default_process(), cpu_id);
        }

        if(next_thread != current_thread){

            context_switch(state, current_thread,  next_thread, next_process->cr3);

            current_thread->thread_state = THREAD_STATE_READY;
            next_thread->thread_state = THREAD_STATE_RUNNING;
        }

    }
}

void schedule_process_waiting(cpu_state_t* state){
    if(info_get_processor_id() != 0) return;

    next_process = process_dequeue();
    
    if(!next_process){
        next_process = default_process;
    }

    schedule_send_int();

    schedule_thread(state);

    if(next_process){
        if(current_process){
            current_process->process_state = PROCESS_STATE_WAITING;
        }

        next_process->process_state = PROCESS_STATE_RUNNING;
        current_process = next_process;

    }
}

void schedule_thread_waiting(cpu_state_t* state){
    uint32_t cpu_id = info_get_processor_id();

    thread_t* current_thread = get_process_thread(current_process, cpu_id);
    thread_t* next_thread = get_process_thread(default_process, cpu_id);

    if(current_thread->thread_state == THREAD_STATE_RUNNING){
        context_switch(state, current_thread, next_thread, default_process->cr3);
        current_thread->thread_state = THREAD_STATE_WAITING;
        current_process->n_active_threads --;
        next_thread->thread_state = THREAD_STATE_RUNNING;
        
    }
}

void schedule_process_terminated(cpu_state_t* state){
    if(info_get_processor_id() != 0) return;

    pcb_t* next_process = process_dequeue();

    if(!next_process){
        next_process = default_process;
    }

    schedule_send_int();

    schedule_thread(state);

    if(next_process){
        if(current_process){
            current_process->process_state = PROCESS_STATE_TERMINATED;
            remove_process(current_process);
        }

        next_process->process_state = PROCESS_STATE_RUNNING;
        current_process = next_process;

    }
}

void schedule_thread_terminated(cpu_state_t* state){
    uint32_t cpu_id = info_get_processor_id();

    if(cpu_id == 0){
        schedule_process_terminated(state);
        return;
    }

    thread_t* current_thread = get_process_thread(current_process, cpu_id);
    thread_t* next_thread = get_process_thread(default_process, cpu_id);

    if(current_thread->thread_state == THREAD_STATE_RUNNING){
        context_switch(state, current_thread, next_thread, default_process->cr3);
        remove_thread(current_process, current_thread);
        next_thread->thread_state = THREAD_STATE_RUNNING;

    }else if(current_thread->thread_state == THREAD_STATE_WAITING){
        remove_thread(current_process, current_thread);
    }
}

void context_switch(cpu_state_t* cpu, thread_t* current_thread, thread_t* next_thread, uint32_t cr3){

    if(current_thread){
        current_thread->cpu_state = *cpu;
    }
    next_thread->cpu_state.esp = cpu->esp;
    *cpu = next_thread->cpu_state;

    asm volatile("mov %0, %%cr3" :: "r"(cr3));
}