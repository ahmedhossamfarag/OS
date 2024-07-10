#include "tss.h"
#include "gdt.h"

tss_entry tss;

void init_tss(){
    uint32_t base = (uint32_t)&tss;
    uint32_t limit = base + sizeof(tss_entry);

    for (char* c = (char*)base; c < (char*)limit; c++)
    {
        *c = 0;
    }
    
    tss.ss0 = KERNEL_DATA_SEGMENT;  // Set the kernel data segment selector
    tss.esp0 = KERNEK_STACK_POINTER;        // Set the kernel stack pointer

    set_gdt_entry(TSS_INDEX, base, limit, 0xE9, 0x00);  // 0xE9 is the access byte for TSS

}

void load_tss(){
    asm volatile ("ltr %0" : : "r" (TSS_SELECTOR)); // This will be a function to load the TSS descriptor into the Task Register
}