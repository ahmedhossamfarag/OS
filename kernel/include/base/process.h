#include <stdint.h>

typedef enum {
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_READY,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_TERMINATED
} process_state_t;

typedef struct {
    uint32_t pid;                 // Process ID
    uint32_t ppid;                // Parent Process ID
    uint32_t state;               // Process state
    uint32_t priority;            // Priority of the process
    uint32_t program_counter;     // Program counter
    uint32_t registers[32];       // CPU registers (simplified)
    uint32_t base_register;       // Base register for memory management
    uint32_t limit_register;      // Limit register for memory management
    uint32_t page_table_base;     // Base address of the page table
    uint32_t open_files[10];      // List of open files (simplified)
    uint32_t cpu_usage;           // CPU usage accounting
    uint32_t creation_time;       // Process creation time
} pcb_t;
