#include "pagging.h"

page_table_t kernel_page_table __attribute__((aligned(PAGE_SIZE)));

void init_page_tables() {
    for (int i = 0; i < NUM_PAGES; i++) {
        kernel_page_table.entries[i].present = 1;
        kernel_page_table.entries[i].rw = 1;
        kernel_page_table.entries[i].user = 0;
        kernel_page_table.entries[i].pwt = 0;
        kernel_page_table.entries[i].pcd = 0;
        kernel_page_table.entries[i].accessed = 0;
        kernel_page_table.entries[i].dirty = 0;
        kernel_page_table.entries[i].pat = 0;
        kernel_page_table.entries[i].global = 0;
        kernel_page_table.entries[i].ignored = 0;
        kernel_page_table.entries[i].frame = i;
    }
    enable_paging();
}


void enable_paging() {
    // Load the page directory address into CR3
    asm volatile("mov %0, %%cr3" :: "r"(&kernel_page_table));

    // Enable paging (set the PG bit in CR0)
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

/* INT 14 */
void page_fault_handler() {
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r"(faulting_address));

    // Determine cause and handle page fault
    // Example: load page from disk if not present
}