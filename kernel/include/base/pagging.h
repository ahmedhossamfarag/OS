#define PAGE_SIZE 4096
#define NUM_PAGES 1024
#define NUM_FRAMES 1024

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


void init_page_tables();

void enable_paging();

/* INT 14 */
void page_fault_handler();