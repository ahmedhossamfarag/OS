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
    schedule_thread_terminated(cpu);
}

void pic_handler(void) {
    uint16_t isr = pic_get_isr();
    uint8_t master_isr = isr & 0xFF;
    uint8_t slave_isr = (isr >> 8) & 0xFF;

    uint16_t irr = pic_get_irr();
    uint8_t master_irr = irr & 0xFF;
    uint8_t slave_irr = (irr >> 8) & 0xFF;

    if(master_isr){
        pic_sendEOI(0);
    }
    if(slave_isr){
        pic_sendEOI(8);
    }

    print("pic ");
    char s[10];
    print(int_to_hex_str(isr, s));
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