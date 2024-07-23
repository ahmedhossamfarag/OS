#include "interrupt_handler.h"
#include "screen_print.h"
#include "pic.h"
#include "interrupt.h"
#include "apic.h"
#include "strlib.h"


void handler(){
    print("done");
    while (1);
}

extern void isr_syscall_handler();

void interrupt_handler_init()
{
    idt_set_user_entry(0x80, (uint32_t)isr_syscall_handler);

    idt_set_user_entry(0, (uint32_t)handler);
}

void exception_handler(cpu_state_t* cpu)
{
    screen_print_str("\nException Handler");
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

    screen_print_str("pic ");
    char s[10];
    screen_print_str(int_to_hex_str(isr, s));
}
