#include "interrupt_handler.h"
#include "screen_print.h"
#include "pic.h"
#include "interrupt.h"

void handler(){
    print("Done");
    while (1)
    {
        /* code */
    }
    
}

void interrupt_handler_init()
{
    set_idt_entry_user(0, (uint32_t)handler);
}

void exception_handler(void)
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
        pic_sendEOI(master_irr);
    }
    if(slave_isr){
        pic_sendEOI(slave_irr);
    }
}
