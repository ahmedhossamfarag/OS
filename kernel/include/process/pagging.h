#define PAGE_SIZE 4096  // 4KB page size
#define NUM_PAGES 512   // Corresponds to 2MB of address space with 4KB pages
#define NUM_PAGE_TABLES 4   // Corresponds to 4GB of address space with 4KB pages

#define FIRST_DIR_ALIGN 0XB001000
#define PROCESS_N_PAGE_TABLES 0x4
#define N_DIRS 8

#define KERNEL_PRIVILEGE 3
#define USER_PRIVILEGE 7

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


void pagging_init();

void enable_paging();

uint64_t* get_default_pagging_dir();

uint64_t* get_available_pagging_dir();

void free_pagging_dir(uint64_t* dir);

uint64_t virtual_to_physical(uint64_t virtual_address, uint64_t* paging_dir);

/* INT 14 */
void page_fault_handler();