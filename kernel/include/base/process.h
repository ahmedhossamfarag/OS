#pragma once
#include <stdint.h>

typedef enum {
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_READY,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_TERMINATED
} process_state_t;

typedef struct {
    uint32_t ds, es, fs, gs;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t eip, cs, eflags;
} cpu_state_t;


typedef struct {
    uint32_t pid;                 // Process ID
    uint32_t ppid;                // Parent Process ID
    cpu_state_t cpu_state;               // CPU state
    process_state_t process_state;        // Process state
} pcb_t;
