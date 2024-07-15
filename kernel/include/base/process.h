#pragma once

#define USER_CS 0x18 | 3
#define USER_DS 0x20 | 3

#define EFLAGS_DEFAULT 0x200

#include <stdint.h>

typedef enum {
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_WAITING,
    PROCESS_STATE_READY,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_TERMINATED
} process_state_t;

typedef struct {
    uint32_t ds, es, fs, gs;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t eip, cs, eflags, user_esp, user_ss;
} cpu_state_t;


typedef struct
{
    uint32_t ds, es, fs, gs, ss;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t eip, cs, eflags;
} registers_t;


typedef struct {
    uint32_t pid;                 // Process ID
    uint32_t ppid;                // Parent Process ID
    process_state_t process_state;        // Process state
    cpu_state_t cpu_state;
    uint32_t cr3;
} pcb_t;

pcb_t create_process(uint32_t pid, uint32_t ppid, uint32_t cr3, uint32_t ebp);
