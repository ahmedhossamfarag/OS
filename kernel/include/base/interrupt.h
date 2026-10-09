#define IDT_ENTRIES 256

#define DIVIDE_ERROR_INT 0
#define DEBUG_EXCEPTION_INT 1
#define NMI_INT 2
#define BREAKPOINT_INT 3
#define OVERFLOW_INT 4
#define BOUND_RANGE_EXCEEDED_INT 5
#define INVALID_OPCODE_INT 6
#define DEVICE_NOT_AVAILABLE_INT 7
#define DOUBLE_FAULT_INT 8
#define COPROCESSOR_SEGMENT_OVERRUN_INT 9
#define INVALID_TSS_INT 10
#define SEGMENT_NOT_PRESENT_INT 11
#define STACK_SEGMENT_FAULT_INT 12
#define GENERAL_PROTECTION_FAULT_INT 13
#define PAGE_FAULT_INT 14
#define X87_FLOATING_POINT_INT 16
#define ALIGNMENT_CHECK_INT 17
#define MACHINE_CHECK_INT 18
#define SIMD_EXCEPTION_INT 19
#define VIRTUALIZATION_EXCEPTION_INT 20

#define SYSCALL_INT 0x80

#include <stdint.h>

typedef struct{
    uint16_t offset_low;     // Lower 16 bits of handler function address
    uint16_t selector;       // Code segment selector in GDT
    uint8_t ist;            // Offset into the Interrupt Stack Table, if zero, it is not used.
    uint8_t type_attr;       // Descriptor type and attributes
    uint16_t offset_middle;    // Middle 16 bits of handler function address
    uint32_t offset_high;        // Upper 32 bits of handler function address
    uint32_t zero;            // Reserved
} __attribute__((packed)) idt_entry_t;

typedef struct{
    uint16_t limit;          // Size of IDT
    uint64_t base;           // Base address of IDT
} __attribute__((packed)) idt_pointer_t;

void idt_set_entry(int n, uint64_t handler);

void idt_set_user_entry(int n, uint64_t handler);

void idt_init();

void enable_idt();

void enable_interrupt();

void disable_interrupt();