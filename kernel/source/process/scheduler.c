#include "scheduler.h"
#include "info.h"
#include "apic.h"
#include "interrupt.h"
#include "int_map.h"
#include "memory.h"

extern pcb_t* default_process;

thread_t** current_thread;

pcb_t* get_current_process(){
    return (pcb_t*) get_current_thread()->parent;
}

thread_t *get_current_thread()
{
    return current_thread[info_get_processor_id()];
}

extern void isr_schedule_thread();
extern void isr_schedule_process_waiting();
extern void isr_schedule_thread_waiting();
extern void isr_schedule_process_terminated();
extern void isr_schedule_thread_terminated();

void scheduler_init()
{
    process_init();
    uint8_t n = info_get_processor_no();
    current_thread = (thread_t**) alloc(n*sizeof(thread_t*));
    for (uint8_t i = 0; i < n; i++)
    {
        current_thread[i] = get_process_thread(default_process, i);
    }
    

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

static void schedule_send_int(){
    for (uint8_t apic_id = 1; apic_id < info_get_processor_no(); apic_id++)
    {
        apic_send_ipi(apic_id, SCHEDULE_INT);
    }
}

void schedule(cpu_state_t* state)
{
    schedule_send_int();
    schedule_thread(state);   
}

static void set_current_thread(thread_t* thread){
    current_thread[info_get_processor_id()] = thread;
}

void schedule_thread(cpu_state_t* state){
    thread_t* current = get_current_thread();
    thread_t* next = thread_dequeue();
    if(next){
        current->thread_state = THREAD_STATE_READY;
        if(current->parent != default_process){
            thread_inqueue(current);
        }
        context_switch(state, current,  next, ((pcb_t*)next->parent)->cr3);
        set_current_thread(next);
    }
}

void schedule_process_waiting(cpu_state_t* state){
    schedule_thread_waiting(state);
}

void schedule_thread_waiting(cpu_state_t* state){
    thread_t* current = get_current_thread();
    thread_t* next = thread_dequeue();
    if(!next){
        next = get_process_thread(default_process ,info_get_processor_id());
    }
    thread_waiting(current);
    context_switch(state, current,  next, ((pcb_t*)next->parent)->cr3);
    set_current_thread(next);
}

void schedule_process_terminated(cpu_state_t* state){
    if(info_get_processor_id() != 0) return;

    pcb_t* p = get_current_process();

    for (thread_t* t = p->threads; t < p->threads + MAX_N_THREAD; t++)
    {
        if(t->thread_state == THREAD_STATE_RUNNING && t->processor_id != 0){
            apic_send_ipi(t->processor_id, THREAD_TERMINATED_INT);
        }
    }

    schedule_thread_terminated(state);
}

void schedule_thread_terminated(cpu_state_t* state){
    thread_t* current = get_current_thread();
    thread_t* next = thread_dequeue();
    if(!next){
        next = get_process_thread(default_process ,info_get_processor_id());
    }
    remove_thread((pcb_t*)current->parent, current);
    context_switch(state, current,  next, ((pcb_t*)next->parent)->cr3);
    set_current_thread(next);
}

void context_switch(cpu_state_t* cpu, thread_t* current_thread, thread_t* next_thread, uint32_t cr3){

    if(current_thread){
        current_thread->cpu_state = *cpu;
    }
    next_thread->cpu_state.esp = cpu->esp;
    *cpu = next_thread->cpu_state;

    asm volatile("mov %0, %%cr3" :: "r"(cr3));
}