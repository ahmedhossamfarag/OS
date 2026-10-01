#include "pagging.h"
#include "interrupt.h"
#include "pages.h"

uint64_t* default_dir;

uint64_t* user_dirs[N_DIRS];
uint8_t user_dir_available[N_DIRS];

extern void isr_page_fault_handler();

void pagging_init() {
    pages_init();

    uint64_t align = FIRST_DIR_ALIGN;

    // default pagging directory

    default_dir = (uint64_t*) align;
    align += PAGE_SIZE;

    uint64_t *pagging_table, *dir_table;

    for (uint32_t k = 0; k < NUM_PAGE_TABLES; k++){
        dir_table = (uint64_t*) align;
        align += PAGE_SIZE;

        for (uint32_t j = 0; j < NUM_PAGES; j++)
        {
            pagging_table = (uint64_t*) align;
            align += PAGE_SIZE;

            for (int i = 0; i < NUM_PAGES; i++) {
                // Set the page table entry to map to the physical address
                pagging_table[i] = ((i << 12) + (j << 21) + (k << 30)) | KERNEL_PRIVILEGE; // Present, Read/Write, Supervisor
            }

            dir_table[j] = ((uint64_t)pagging_table) | KERNEL_PRIVILEGE;
        }

        default_dir[k] = ((uint64_t)dir_table) | KERNEL_PRIVILEGE;
    }

    // Set the rest of the entries in the default directory to 0 (not present)
    for (uint32_t i = NUM_PAGE_TABLES; i < NUM_PAGES; i++) {
        default_dir[i] = 0;
    }

    // First Dir Shift
    ((uint64_t*)default_dir[0xF0000000 >> 30])[0xF0000000 >> 21] = default_dir[0];
    
    // user pagging directories
    uint32_t frame = 0;

    for (uint8_t j = 0; j < N_DIRS; j++)
    {
        user_dirs[j] = (uint64_t*) align;
        align += PAGE_SIZE;

        
        for (uint32_t i = 0; i < NUM_PAGES; i++)
        {
            user_dirs[j][i] = default_dir[i];
        }

        dir_table = (uint64_t*) align;
        align += PAGE_SIZE;

        for (uint32_t i = 0; i < NUM_PAGES; i++)
        {
            dir_table[i] = ((uint64_t*)default_dir[0])[i];
        }

        for (uint32_t i = 0; i < PROCESS_N_PAGE_TABLES; i++)
        {
            pagging_table = (uint64_t*) align;
            align += PAGE_SIZE;

            frame = pages_alloc_next(frame);

            for (int k = 0; k < NUM_PAGES; k++)
            {
                pagging_table[k] = frame | USER_PRIVILEGE;
                frame += PAGE_SIZE;
            }

            dir_table[i] = ((uint64_t)pagging_table) | USER_PRIVILEGE;
        }

        user_dirs[j][0] = ((uint64_t)dir_table) | KERNEL_PRIVILEGE;
        

        user_dir_available[j] = 1;
    }


    idt_set_entry(14, (uint64_t)(isr_page_fault_handler));

}


void enable_paging() {
    // Load the page directory address into CR3
    asm volatile("mov %0, %%cr3" :: "r"(default_dir));

    // Enable paging (set the PG bit in CR0)
    uint64_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

uint64_t *get_default_pagging_dir()
{
    return default_dir;
}

uint64_t* get_available_pagging_dir()
{
    for (uint8_t i = 0; i < N_DIRS; i++)
    {
        if(user_dir_available[i]){
            user_dir_available[i] = 0;
            return user_dirs[i];
        }
    }
    return 0;
}

void free_pagging_dir(uint64_t *dir)
{
    for (uint8_t i = 0; i < N_DIRS; i++)
    {
        if(dir == user_dirs[i]){
            user_dir_available[i] = 1;
            return;
        }
    }
    
}

uint64_t virtual_to_physical(uint64_t virtual_address, uint64_t* paging_dir) {
    #define PAGE_DIR_TABLE_INDEX(va) (((va) >> 30) & 0x1FF)
    #define PAGE_DIR_INDEX(va) (((va) >> 21) & 0x1FF)
    #define PAGE_TABLE_INDEX(va) (((va) >> 12) & 0x1FF)
    #define PAGE_OFFSET(va) ((va) & 0xFFF)

    #define PAGE_MASK 0xFFFFFFFFFF000

    uint64_t dt_index = PAGE_DIR_TABLE_INDEX(virtual_address);
    uint64_t pd_index = PAGE_DIR_INDEX(virtual_address);
    uint64_t pt_index = PAGE_TABLE_INDEX(virtual_address);
    
    uint64_t dir_table_base = paging_dir[dt_index] & PAGE_MASK;

    uint64_t* dir_table = (uint64_t*)dir_table_base;

    uint64_t page_table_base = dir_table[pd_index] & PAGE_MASK;
    
    uint64_t* page_table = (uint64_t*)page_table_base;

    uint64_t physical_page_base = page_table[pt_index] & PAGE_MASK;
    
    uint64_t physical_address = physical_page_base | PAGE_OFFSET(virtual_address);
    
    return physical_address;
}


/* INT 14 */
void page_fault_handler(uint64_t error_code) {
    uint64_t faulting_address;
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
