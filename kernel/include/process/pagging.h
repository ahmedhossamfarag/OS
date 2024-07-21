#define PAGE_SIZE 4096
#define NUM_PAGES 1024
#define NUM_FRAMES 1024

#define FIRST_DIR_ALIGN 0XC000000
#define FIRST_FRAME 0X400000
#define PROCESS_N_PAGE_TABLES 0x4
#define N_DIRS 4

#define KERNEL_PRIVILEGE 3
#define USER_PRIVILEGE 7

#include <stdint.h>

typedef struct {
    uint32_t present : 1;    // 1 bit
    uint32_t rw : 1;         // 1 bit
    uint32_t user : 1;       // 1 bit
    uint32_t pwt : 1;        // Page-level Write-Through
    uint32_t pcd : 1;        // Page-level Cache Disable
    uint32_t accessed : 1;   // Accessed
    uint32_t dirty : 1;      // Dirty
    uint32_t pat : 1;        // Page Attribute Table index (if PAT is supported)
    uint32_t global : 1;     // Global
    uint32_t ignored : 3;    // Ignored bits (available for software use)
    uint32_t frame : 20;     // Physical frame address (assuming 4 KB pages and 32-bit system)
} page_table_entry_t;

typedef struct {
    page_table_entry_t entries[NUM_PAGES];
} page_table_t;


void pagging_init();

void enable_paging();

uint32_t* get_default_pagging_dir();

uint32_t* get_available_pagging_dir();

void free_pagging_dir(uint32_t* dir);

uint32_t virtual_to_physical(uint32_t virtual_address, uint32_t* paging_dir);

/* INT 14 */
void page_fault_handler();