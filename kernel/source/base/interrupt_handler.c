#include "interrupt_handler.h"
#include "vga_print.h"
#include "pic.h"
#include "interrupt.h"
#include "apic.h"
#include "strlib.h"
#include "syscall_map.h"
#include "scheduler.h"

extern void isr_syscall_handler();

void interrupt_handler_init()
{
    idt_set_user_entry(0x80, (uint32_t)isr_syscall_handler);
}

static void track_exception(cpu_state_t* cpu){
    char s[20];
    print("PID: ")
    println(int_to_hex_str(get_current_process()->pid, s))
    print("TID: ")
    println(int_to_hex_str(get_current_thread()->tid, s))
    print("CR3: ")
    println(int_to_hex_str(get_current_process()->cr3, s))
    print("CS: ")
    println(int_to_hex_str(cpu->cs, s))
    print("EIP: ")
    println(int_to_hex_str(cpu->eip, s))
    print("ESP: ")
    println(int_to_hex_str(cpu->user_esp, s))
}

void exception_handler(cpu_state_t* cpu)
{
    print("\nException Handler\n");
    track_exception(cpu);
    if(get_current_process() != get_default_process()){
        schedule_thread_terminated(cpu);
    }   
    pic_sendEOI_helper();
}


void pic_handler(void) {
    print("\nPIC Handler\n");
    pic_sendEOI_helper();
}

extern void (*syscall_map[NUM_SYSCALL])(cpu_state_t*);

void syscall_handler(cpu_state_t* state){
    uint32_t n;
    asm("mov %%esi, %0":"=m"(n));

    if(n >= NUM_SYSCALL || !syscall_map[n]){
        state->eax = 0;
        return;
    }
    syscall_map[n](state);
}

void gp_fault_handler(){
}