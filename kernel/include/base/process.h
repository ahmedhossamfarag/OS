#pragma once

#define USER_CS 0x18
#define USER_DS 0x20
#define PROCESS_STACK_SIZE 0x1000 

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
    uint32_t edi, esi, ebp, _, ebx, edx, ecx, eax;
    uint32_t eip, cs, eflags;
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
    registers_t registers;         // CPU state
    process_state_t process_state;        // Process state
} pcb_t;

pcb_t create_process(uint32_t pid, uint32_t ppid, uint32_t eip, uint32_t ebp);
