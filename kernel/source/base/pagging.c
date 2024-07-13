#include "pagging.h"
#include "screen_print.h"
#include "interrupt.h"

uint32_t* pagging_dir;
uint32_t* pagging_table;

extern void isr_page_fault_handler();

void init_page_tables() {
    pagging_dir = (uint32_t*) 0x20000;
    for (uint32_t* i = pagging_dir; i < pagging_dir + NUM_PAGES; i++)
    {
        *i = 0;
    }

    pagging_table = (uint32_t*) 0x21000;
    
    for (int i = 0; i < NUM_PAGES; i++) {
        // Set the page table entry to map to the physical address
        pagging_table[i] = (i * PAGE_SIZE) | 3; // Present, Read/Write, Supervisor
    }

    pagging_dir[0] = ((uint32_t)pagging_table) | 3;

    set_idt_entry(14, (uint32_t)(isr_page_fault_handler));

    enable_paging();
}


void enable_paging() {
    // Load the page directory address into CR3
    asm volatile("mov %0, %%cr3" :: "r"(pagging_dir));

    // Enable paging (set the PG bit in CR0)
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

/* INT 14 */
void page_fault_handler(uint32_t error_code) {
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r"(faulting_address));

    // Analyze the faulting address and error code
    if (!(error_code & 0x1)) {
        // Page not present
    } else {
        // Page protection violation
        if (error_code & 0x2) {
            // Write operation
        } else {
            // Read operation
        }
        if (error_code & 0x4) {
            // Fault occurred in user mode
        } else {
            // Fault occurred in kernel mode
        }
    }
}
