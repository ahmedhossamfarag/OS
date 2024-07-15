#include "pagging.h"
#include "interrupt.h"

uint32_t* default_dir;

uint32_t* user_dirs[4];
uint8_t user_dir_available[4];

extern void isr_page_fault_handler();

void init_page_tables() {
    uint32_t align = 0x20000;

    // default pagging directory

    default_dir = (uint32_t*) align;
    align += PAGE_SIZE;

    
    for (uint32_t* i = default_dir; i < default_dir + NUM_PAGES; i++)
    {
        *i = 0;
    }

    uint32_t* pagging_table = (uint32_t*) align;
    align += PAGE_SIZE;

    for (int i = 0; i < NUM_PAGES; i++) {
        // Set the page table entry to map to the physical address
        pagging_table[i] = (i * PAGE_SIZE) | KERNEL_PRIVILEGE; // Present, Read/Write, Supervisor
    }

    default_dir[0] = ((uint32_t)pagging_table) | KERNEL_PRIVILEGE;


    // user pagging directories
    uint32_t frame = 0x40000;

    for (uint8_t j = 0; j < 4; j++)
    {
        user_dirs[j] = (uint32_t*) align;
        align += PAGE_SIZE;

        
        for (uint32_t* i = user_dirs[j]; i < user_dirs[j] + NUM_PAGES; i++)
        {
            *i = 0;
        }

        pagging_table = (uint32_t*) align;
        align += PAGE_SIZE;

        for (int i = 0; i < 1; i++)
        {
            pagging_table[i] = frame | USER_PRIVILEGE;
            frame += PAGE_SIZE;
        }
        

        for (int i = 1; i < NUM_PAGES; i++) {
            // Set the page table entry to map to the physical address
            pagging_table[i] = (i * PAGE_SIZE) | KERNEL_PRIVILEGE; // Present, Read/Write, Supervisor
        }

        user_dirs[j][0] = ((uint32_t)pagging_table) | USER_PRIVILEGE;

        user_dir_available[j] = 1;
    }


    set_idt_entry(14, (uint32_t)(isr_page_fault_handler));

    enable_paging();
}


void enable_paging() {
    // Load the page directory address into CR3
    asm volatile("mov %0, %%cr3" :: "r"(default_dir));

    // Enable paging (set the PG bit in CR0)
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

uint32_t *get_default_pagging_dir()
{
    return default_dir;
}

uint32_t* get_available_pagging_dir()
{
    for (uint8_t i = 0; i < 4; i++)
    {
        if(user_dir_available[i]){
            user_dir_available[i] = 0;
            return user_dirs[i];
        }
    }
    return 0;
}

void free_pagging_dir(uint32_t *dir)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        if(dir == user_dirs[i]){
            user_dir_available[i] = 1;
            return;
        }
    }
    
}

uint32_t virtual_to_physical(uint32_t virtual_address, uint32_t* paging_dir) {
    #define PAGE_DIR_INDEX(va) (((va) >> 22) & 0x3FF)
    #define PAGE_TABLE_INDEX(va) (((va) >> 12) & 0x3FF)
    #define PAGE_OFFSET(va) ((va) & 0xFFF)

    #define PAGE_MASK 0xFFFFF000

    uint32_t pd_index = PAGE_DIR_INDEX(virtual_address);
    uint32_t pt_index = PAGE_TABLE_INDEX(virtual_address);
    
    uint32_t page_table_base = paging_dir[pd_index] & PAGE_MASK;
    
    uint32_t* page_table = (uint32_t*)page_table_base;

    uint32_t physical_page_base = page_table[pt_index] & PAGE_MASK;
    
    uint32_t physical_address = physical_page_base | PAGE_OFFSET(virtual_address);
    
    return physical_address;
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
