#include "paging.h"
#include "interrupt.h"
#include "pages.h"
#include "libc.h"
#include "info.h"
#include "memory.h"


uint64_t *default_paging;

uint64_t *user_paging[PAGING_N_TABLES];
uint8_t user_paging_available[PAGING_N_TABLES];

extern void isr_page_fault_handler();


// Allocate one 4 KiB page for a paging structure.
static uint64_t *alloc_page_table()
{
    uint64_t *table = (uint64_t*) alloc_align(PAGE_4K, PAGE_4K);

    return table;
}


static void virtual_memory_mapping(uint64_t* pml4){
    uint32_t pml4_index = (MEMORY_VIRTUAL_START >> 39) & 0x1FF;
    uint64_t* pdpt = alloc_page_table();
    mem_set((char*)pdpt, 0, PAGE_SIZE);
    pml4[pml4_index] = ((uint64_t)pdpt) | KERNEL_PRIVILEGE;

    for (
        uint32_t pdpt_index = (MEMORY_VIRTUAL_START >> 30) & 0x1FF;
        pdpt_index < ENTRIES_PER_TABLE; 
        pdpt_index++
    ) {
        uint64_t* pd = alloc_page_table();
        mem_set((char*)pd, 0, PAGE_SIZE);
        pdpt[pdpt_index] = ((uint64_t)pd) | KERNEL_PRIVILEGE;

        for (
            uint32_t pd_index = (MEMORY_VIRTUAL_START >> 21) & 0x1FF; 
            pd_index < ENTRIES_PER_TABLE; 
            pd_index++
        ) {
            uint64_t virtual_address =
                ((uint64_t)pml4_index << 39) |
                ((uint64_t)pdpt_index << 30) |
                ((uint64_t)pd_index   << 21);
            uint64_t physical_address = MEMORY_VIRT_TO_PHYS(virtual_address);
            pd[pd_index] =
                physical_address |
                KERNEL_PRIVILEGE |
                PAGE_PS;
        }
    }
}

void paging_init()
{
    pages_init();

    /*
     * ----------------------------------------------------------------------
     * DEFAULT KERNEL ADDRESS SPACE
     *
     * PML4
     *   └── PDPT
     *        └── PD
     *             └── 2 MiB pages
     *
     * This creates an identity mapping.
     * ----------------------------------------------------------------------
     */

    uint64_t *pml4;
    uint64_t *pdpt;

    pml4 = alloc_page_table();
    pdpt = alloc_page_table();

    mem_set((char*)pml4, 0, PAGE_SIZE);
    mem_set((char*)pdpt, 0, PAGE_SIZE);

    default_paging = pml4;

    pml4[0] = ((uint64_t)pdpt) | KERNEL_PRIVILEGE;

    for (uint32_t pdpt_index = 0; pdpt_index < NUM_PDPT_ENTRIES; pdpt_index++)
    {
        uint64_t *pd = alloc_page_table();

        mem_set((char*)pd, 0, PAGE_SIZE);

        for (uint32_t pd_index = 0; pd_index < ENTRIES_PER_TABLE; pd_index++)
        {
            uint64_t physical_address =
                ((uint64_t)pdpt_index << 30) |
                ((uint64_t)pd_index   << 21);

            pd[pd_index] =
                physical_address |
                KERNEL_PRIVILEGE |
                PAGE_PS;
        }

        pdpt[pdpt_index] =
            ((uint64_t)pd) |
            KERNEL_PRIVILEGE;
    }

    virtual_memory_mapping(pml4);

    /*
     * ----------------------------------------------------------------------
     * USER ADDRESS SPACES
     * ----------------------------------------------------------------------
     *
     * Each user directory gets its own PML4.
     *
     * The kernel mappings are copied into it.
     *
     * User memory is installed in PDs using 2 MiB frames.
     */

    uint64_t frame = 0;

    for (uint8_t j = 0; j < PAGING_N_TABLES; j++)
    {
        uint64_t *user_pml4;
        uint64_t *user_pdpt;

        user_pml4 = alloc_page_table();
        user_pdpt = alloc_page_table();

        mem_set((char*)user_pml4, 0, PAGE_SIZE);
        mem_set((char*)user_pdpt, 0, PAGE_SIZE);

        user_paging[j] = user_pml4;

        user_pml4[0] = ((uint64_t)user_pdpt) | USER_PRIVILEGE;

        for (uint32_t pdpt_index = 0; pdpt_index < NUM_PDPT_ENTRIES; pdpt_index++)
        {
            uint64_t *pd = alloc_page_table();

            mem_set((char*)pd, 0, PAGE_SIZE);

            for (uint32_t pd_index = 0; pd_index < ENTRIES_PER_TABLE; pd_index++)
            {
                if (pdpt_index == 0 && pd_index < PROCESS_N_PD_ENTRIES)
                {
                    frame = pages_alloc_next(frame);

                    pd[pd_index] =
                        frame |
                        USER_PRIVILEGE |
                        PAGE_PS;
                } else {
                    uint64_t physical_address =
                        ((uint64_t)pdpt_index << 30) |
                        ((uint64_t)pd_index   << 21);

                    pd[pd_index] =
                        physical_address |
                        KERNEL_PRIVILEGE |
                        PAGE_PS;
                }
            }

            if (pdpt_index == 0){
                user_pdpt[pdpt_index] =
                    ((uint64_t)pd) |
                    USER_PRIVILEGE;
            } else {
                user_pdpt[pdpt_index] =
                ((uint64_t)pd) |
                KERNEL_PRIVILEGE;
            }
        }

        virtual_memory_mapping(user_pml4);

        user_paging_available[j] = 1;
    }


    /*
     * Page fault handler.
     */
    idt_set_entry( 14, (uint64_t)isr_page_fault_handler);
}


/*
 * --------------------------------------------------------------------------
 * Enable paging
 * --------------------------------------------------------------------------
 */

void enable_paging()
{
    /*
     * CR3 must contain the PHYSICAL address of the PML4.
     *
     * default_paging is currently identity mapped.
     */
    uint64_t cr3 = (uint64_t)default_paging;

    asm volatile (
        "mov %0, %%cr3"
        :
        : "r"(cr3)
        : "memory"
    );


    /*
     * Enable PAE.
     *
     * Required before enabling long-mode paging.
     *
     * If your boot code already enabled PAE, this is harmless.
     */
    uint64_t cr4;

    asm volatile (
        "mov %%cr4, %0"
        : "=r"(cr4)
    );

    cr4 |= (1ULL << 5);     /* CR4.PAE */

    asm volatile (
        "mov %0, %%cr4"
        :
        : "r"(cr4)
        : "memory"
    );


    /*
     * Enable paging.
     */

    uint64_t cr0;

    asm volatile (
        "mov %%cr0, %0"
        : "=r"(cr0)
    );

    cr0 |= (1ULL << 31);    /* CR0.PG */

    asm volatile (
        "mov %0, %%cr0"
        :
        : "r"(cr0)
        : "memory"
    );
}


uint64_t *get_default_paging()
{
    return default_paging;
}


uint64_t *get_available_user_paging()
{
    for (uint8_t i = 0; i < PAGING_N_TABLES; i++)
    {
        if (user_paging_available[i])
        {
            user_paging_available[i] = 0;
            return user_paging[i];
        }
    }

    return 0;
}


void free_user_paging(uint64_t *dir)
{
    for (uint8_t i = 0; i < PAGING_N_TABLES; i++)
    {
        if (dir == user_paging[i])
        {
            user_paging_available[i] = 1;
            return;
        }
    }
}


/*
 * --------------------------------------------------------------------------
 * Virtual -> Physical
 *
 * 2 MiB page version
 * --------------------------------------------------------------------------
 */

uint64_t virtual_to_physical(uint64_t virtual_address, uint64_t *paging_dir)
{
    /*
     * x86-64 4-level paging:
     *
     *       47          39 38          30 29          21 20       0
     *        +------------+--------------+--------------+----------+
     *        |    PML4    |     PDPT     |      PD      |  offset  |
     *        +------------+--------------+--------------+----------+
     */

    uint64_t pml4_index =
        PML4_INDEX(virtual_address);

    uint64_t pdpt_index =
        PDPT_INDEX(virtual_address);

    uint64_t pd_index =
        PD_INDEX(virtual_address);

    uint64_t offset =
        PAGE_2M_OFFSET(virtual_address);


    /*
     * PML4 -> PDPT
     */
    uint64_t pml4_entry =
        paging_dir[pml4_index];

    if (!(pml4_entry & PAGE_PRESENT))
        return 0;

    uint64_t *pdpt =
        (uint64_t *)(pml4_entry & PAGE_ADDR_MASK);


    /*
     * PDPT -> PD
     */
    uint64_t pdpt_entry =
        pdpt[pdpt_index];

    if (!(pdpt_entry & PAGE_PRESENT))
        return 0;

    /*
     * We expect a PD here, not a 1 GiB page.
     *
     * If PS is set at the PDPT level, that would be
     * a 1 GiB page and needs separate handling.
     */
    if (pdpt_entry & PAGE_PS)
    {
        uint64_t physical_base =
            pdpt_entry & 0x000FFFFFC0000000ULL;

        return physical_base |
               (virtual_address & 0x3FFFFFFFULL);
    }

    uint64_t *pd =
        (uint64_t *)(pdpt_entry & PAGE_ADDR_MASK);


    /*
     * PD -> 2 MiB physical page
     */
    uint64_t pd_entry =
        pd[pd_index];

    if (!(pd_entry & PAGE_PRESENT))
        return 0;

    /*
     * We expect PS=1 here.
     */
    if (!(pd_entry & PAGE_PS))
        return 0;

    /*
     * Extract 2 MiB-aligned physical address.
     */
    uint64_t physical_base =
        pd_entry & PAGE_2M_ADDR_MASK;

    return physical_base | offset;
}


/* INT 14 */
void page_fault_handler(uint64_t error_code) {
    uint64_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r"(faulting_address));

    asm volatile("hlt"); // Halt the CPU for debugging purposes

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
