#define IDT_ENTRIES 256


#include <stdint.h>

typedef struct{
    uint16_t offset_low;     // Lower 16 bits of handler function address
    uint16_t selector;       // Code segment selector in GDT
    uint8_t zero;            // Unused, set to 0
    uint8_t type_attr;       // Descriptor type and attributes
    uint16_t offset_high;    // Upper 16 bits of handler function address
} __attribute__((packed)) IDTEntry;

typedef struct{
    uint16_t limit;          // Size of IDT
    uint32_t base;           // Base address of IDT
} __attribute__((packed)) IDTPointer;

void set_idt_entry(int n, uint32_t handler);

void init_idt();

void enable_interrupt();