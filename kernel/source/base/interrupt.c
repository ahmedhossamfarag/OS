#include "interrupt.h"

extern void isr_exception_handler();
extern void isr_default();

IDTEntry idt[IDT_ENTRIES];
IDTPointer idt_ptr;

void set_idt_entry(int n, uint32_t handler) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08; // Kernel code segment
    idt[n].zero = 0;
    idt[n].type_attr = 0x8E; // Interrupt gate, kernel privilege
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}


void set_idt_entry_user(int n, uint32_t handler){
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08; // Kernel code segment
    idt[n].zero = 0;
    idt[n].type_attr = 0xEE; // Interrupt gate , user privilege
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void map_idt_isr(){
    for (int i = 0; i < IDT_ENTRIES; i++)
    {
        set_idt_entry(i, (uint32_t)(isr_exception_handler));
    }
}


void init_idt() {
    idt_ptr.limit = sizeof(IDTEntry) * IDT_ENTRIES - 1;
    idt_ptr.base = (uint32_t)&idt[0];

    map_idt_isr();

    __asm__ volatile ("lidt %0" : : "m"(idt_ptr)); // load the new IDT
}

void enable_interrupt() {
    __asm__ volatile ("sti"); // set the interrupt flag
}
