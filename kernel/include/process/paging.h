#define PAGE_SIZE 4096  // 4KB page size
#define NUM_PAGES 512   // Corresponds to 2MB of address space with 4KB pages
#define NUM_PDPT_ENTRIES 4   // Corresponds to 4GB of address space with 4KB pages

#define PROCESS_N_PD_ENTRIES 0x8    // Corresponds to 16MB of address space with 2MB pages
#define PAGING_N_TABLES 8           // Number of user paging tables

#define KERNEL_PRIVILEGE 3
#define USER_PRIVILEGE 7


#define ENTRIES_PER_TABLE 512

#define PAGE_4K            0x1000ULL
#define PAGE_2M            0x200000ULL

/* x86-64 paging flags */
#define PAGE_PRESENT       (1ULL << 0)
#define PAGE_WRITE         (1ULL << 1)
#define PAGE_USER          (1ULL << 2)
#define PAGE_PS            (1ULL << 7)   /* 2 MiB page */

#define PAGE_ADDR_MASK     0x000FFFFFFFFFF000ULL
#define PAGE_2M_ADDR_MASK  0x000FFFFFFFE00000ULL

/*
 * A 2 MiB page uses:
 *
 *   PML4 index : bits 47:39
 *   PDPT index : bits 38:30
 *   PD index   : bits 29:21
 *   offset     : bits 20:0
 */

#define PML4_INDEX(va)     (((va) >> 39) & 0x1FF)
#define PDPT_INDEX(va)     (((va) >> 30) & 0x1FF)
#define PD_INDEX(va)       (((va) >> 21) & 0x1FF)
#define PAGE_2M_OFFSET(va) ((va) & 0x1FFFFF)

#include <stdint.h>

typedef struct {
    uint64_t present : 1;    // 1 bit
    uint64_t rw : 1;         // 1 bit
    uint64_t user : 1;       // 1 bit
    uint64_t pwt : 1;        // Page-level Write-Through
    uint64_t pcd : 1;        // Page-level Cache Disable
    uint64_t accessed : 1;   // Accessed
    uint64_t dirty : 1;      // Dirty
    uint64_t pat : 1;        // Page Attribute Table index (if PAT is supported)
    uint64_t global : 1;     // Global
    uint64_t ignored : 3;    // Ignored bits (available for software use)
    uint64_t frame : 40;     // Physical frame address (assuming 4 KB pages and 64-bit system)
    uint64_t reserved : 7;   // Reserved bits (must be zero)
    uint64_t protection_key : 4; // Protection Key (if PKU is supported)
    uint64_t execute_disable : 1; // Execute Disable (if NX is supported)
} page_table_entry_t;

typedef struct {
    page_table_entry_t entries[NUM_PAGES];
} page_table_t;


void paging_init();

void enable_paging();

uint64_t* get_default_paging();

uint64_t* get_available_user_paging();

void free_user_paging(uint64_t* dir);

uint64_t virtual_to_physical(uint64_t virtual_address, uint64_t* paging_dir);
