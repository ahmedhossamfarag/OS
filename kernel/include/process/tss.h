#define KERNEL_DATA_SEGMENT 0x10
#define KERNEL_STACK_SIZE 0X1000
#define KERNEK_STACK_POINTER(i) 0XB000000 - (i * KERNEL_STACK_SIZE)
#define TSS_INDEX(i) (5 + i)
#define TSS_SELECTOR(i) TSS_INDEX(i) * 8
#include <stdint.h>

typedef struct  {
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed)) tss_entry_t;

void tss_init();

void enable_tss();