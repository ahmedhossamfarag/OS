#pragma once

#define MAX_N_PROCESS 10
#define MAX_N_THREAD 5

#define KERNEL_CS 0x8
#define KERNEL_DS 0x10
#define USER_CS 0x18 | 3
#define USER_DS 0x20 | 3

#define EFLAGS_DEFAULT 0x200

#include <stdint.h>

typedef enum {
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_WAITING,
    PROCESS_STATE_READY,
    PROCESS_STATE_TERMINATED
} process_state_t;

typedef enum {
    THREAD_STATE_RUNNING,
    THREAD_STATE_WAITING,
    THREAD_STATE_READY,
    THREAD_STATE_TERMINATED
} thread_state_t;

typedef struct {
    uint32_t ds, es, fs, gs;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t eip, cs, eflags, user_esp, user_ss;
} cpu_state_t;


typedef struct{
    uint32_t tid;
    void* parent;
    uint8_t processor_id;
    thread_state_t thread_state;
    cpu_state_t cpu_state;
} thread_t;

typedef struct {
    uint32_t pid;                 // Process ID
    uint32_t ppid;                // Parent Process ID
    process_state_t process_state;        // Process state
    thread_t threads[MAX_N_THREAD];
    uint8_t n_active_threads;
    uint32_t cr3;
} pcb_t;

void process_init();

pcb_t* get_default_process();

uint8_t add_new_process(uint32_t pid, uint32_t ppid, uint32_t cr3, uint32_t ebp);

void remove_process(pcb_t* process);

void process_inqueue(pcb_t* process);

pcb_t* process_dequeue();

uint8_t add_new_thread(pcb_t* process, uint32_t tid, uint32_t eip, uint32_t ebp);

void remove_thread(pcb_t* process, thread_t* thread);

thread_t* get_process_thread(pcb_t* process, uint8_t n);

void process_awake(pcb_t* process);

void thread_awake(thread_t* thread);