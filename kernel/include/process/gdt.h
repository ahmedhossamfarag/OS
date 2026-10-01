#include <stdint.h>

typedef struct {
    uint16_t limit_low;  // The lower 16 bits of the limit
    uint16_t base_low;   // The lower 16 bits of the base
    uint8_t  base_middle_1; // The next 8 bits of the base
    uint8_t  access;     // Access flags, determine what ring this segment can be used in
    uint8_t  granularity; // Granularity and limit flags
    uint8_t  base_middle_2;  // The next 8 bits of the base
    uint32_t base_high;  // The last 32 bits of the base
    uint32_t reserved;    // Reserved, set to 0
} __attribute__((packed)) gdt_entry_t;

typedef struct  {
    uint16_t limit;  // Limit of the GDT
    uint64_t base;   // Base address of the GDT
} __attribute__((packed)) gdt_ptr_t;

void gdt_init();

void enable_gdt();

void set_gdt_entry(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);

