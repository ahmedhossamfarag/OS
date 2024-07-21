#include "gdt.h"
#include "tss.h"
#include "memory.h"
#include "info.h"

uint8_t GDT_N_ENTRIES;

gdt_entry_t* gdt;
gdt_ptr_t gdtp;

void gdt_init() {
    GDT_N_ENTRIES = 5 + info_get_processor_no();

    gdt = (gdt_entry_t*) alloc(GDT_N_ENTRIES * sizeof(gdt_entry_t));

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
    tss_init();

    // GDT pointer

    gdtp.limit = (sizeof(gdt_entry_t) * GDT_N_ENTRIES) - 1;
    gdtp.base = (uint32_t)gdt;

}

void enable_gdt(){

    // Load the GDT
    asm volatile ("lgdt %0" : : "m" (gdtp));


    enable_tss();
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
