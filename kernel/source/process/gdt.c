#include "gdt.h"
#include "tss.h"

gdt_entry gdt[GDT_N_ENTRIES];

void init_gdt() {
    // Null segment
    set_gdt_entry(0, 0, 0, 0, 0);

    // Code segment
    set_gdt_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Data segment
    set_gdt_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // User mode code segment
    set_gdt_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    // User mode data segment
    set_gdt_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    // TSS Initialize
    init_tss();

    // GDT pointer

    gdt_ptr gdtp;
    gdtp.limit = (sizeof(gdt_entry) * GDT_N_ENTRIES) - 1;
    gdtp.base = (uint32_t)&gdt;

    // Load the GDT
    asm volatile ("lgdt %0" : : "m" (gdtp));


    load_tss();
}


void set_gdt_entry(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}
