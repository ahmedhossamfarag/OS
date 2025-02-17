#define IDT_ENTRIES 256


#include <stdint.h>

typedef struct{
    uint16_t offset_low;     // Lower 16 bits of handler function address
    uint16_t selector;       // Code segment selector in GDT
    uint8_t zero;            // Unused, set to 0
    uint8_t type_attr;       // Descriptor type and attributes
    uint16_t offset_high;    // Upper 16 bits of handler function address
} __attribute__((packed)) idt_entry_t;

typedef struct{
    uint16_t limit;          // Size of IDT
    uint32_t base;           // Base address of IDT
} __attribute__((packed)) idt_pointer_t;

void idt_set_entry(int n, uint32_t handler);

void idt_set_user_entry(int n, uint32_t handler);

void idt_init();

void enable_idt();

void enable_interrupt();