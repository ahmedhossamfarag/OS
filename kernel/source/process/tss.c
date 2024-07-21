#include "tss.h"
#include "gdt.h"
#include "memory.h"
#include "info.h"

uint8_t n_tss;
tss_entry_t* tss;

void tss_init(){
    n_tss = info_get_processor_no();

    tss = (tss_entry_t*) alloc(n_tss * sizeof(tss_entry_t));

    for (uint8_t i = 0; i < n_tss; i++)
    {
        uint32_t base = (uint32_t)&tss[i];
        uint32_t limit = base + sizeof(tss_entry_t);

        for (char* c = (char*)base; c < (char*)limit; c++)
        {
            *c = 0;
        }
        
        tss[i].ss0 = KERNEL_DATA_SEGMENT;  // Set the kernel data segment selector
        tss[i].esp0 = KERNEK_STACK_POINTER(i);        // Set the kernel stack pointer

        set_gdt_entry(TSS_INDEX(i), base, limit, 0xE9, 0x00);  // 0xE9 is the access byte for TSS
    }
    

}

void enable_tss(){
    uint32_t selector = TSS_SELECTOR(info_get_processor_id());
    asm volatile ("ltr %0" : : "r" (selector)); // This will be a function to load the TSS descriptor into the Task Register
}