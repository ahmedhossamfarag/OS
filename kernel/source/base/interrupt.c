#include "interrupt.h"
#include "memory.h"
extern void isr_exception_handler();
extern void isr_default();

idt_entry_t* idt;
idt_pointer_t idt_ptr;

void idt_set_entry(int n, uint32_t handler) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08; // Kernel code segment
    idt[n].zero = 0;
    idt[n].type_attr = 0x8E; // Interrupt gate, kernel privilege
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}


void idt_set_user_entry(int n, uint32_t handler){
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08; // Kernel code segment
    idt[n].zero = 0;
    idt[n].type_attr = 0xEE; // Interrupt gate , user privilege
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void map_idt_isr(){
    for (int i = 0; i < IDT_ENTRIES; i++)
    {
        idt_set_entry(i, (uint32_t)(isr_exception_handler));
    }
}


void idt_init() {
    idt = (idt_entry_t*) alloc(IDT_ENTRIES * sizeof(idt_entry_t));

    idt_ptr.limit = sizeof(idt_entry_t) * IDT_ENTRIES - 1;
    idt_ptr.base = (uint32_t)&idt[0];

    map_idt_isr();
}

void enable_idt(){
    __asm__ volatile ("lidt %0" : : "m"(idt_ptr)); // load the new IDT
}

void enable_interrupt() {
    __asm__ volatile ("sti"); // set the interrupt flag
}
