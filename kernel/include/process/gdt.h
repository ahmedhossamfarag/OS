#define GDT_N_ENTRIES 6

#include <stdint.h>

typedef struct {
    uint16_t limit_low;  // The lower 16 bits of the limit
    uint16_t base_low;   // The lower 16 bits of the base
    uint8_t  base_middle; // The next 8 bits of the base
    uint8_t  access;     // Access flags, determine what ring this segment can be used in
    uint8_t  granularity; // Granularity and limit flags
    uint8_t  base_high;  // The last 8 bits of the base
} __attribute__((packed)) gdt_entry;

typedef struct  {
    uint16_t limit;  // Limit of the GDT
    uint32_t base;   // Base address of the GDT
} __attribute__((packed)) gdt_ptr;

void init_gdt();


void set_gdt_entry(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);

