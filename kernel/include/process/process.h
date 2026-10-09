#pragma once

#define MAX_N_PROCESS 10
#define MAX_N_THREAD 5

#define KERNEL_CS 0x10
#define KERNEL_DS 0x20
#define USER_CS 0x30 | 3
#define USER_DS 0x40 | 3

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
    uint64_t ds, es, fs, gs;
    uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
    uint64_t rip, cs, rflags, user_rsp, user_ss;
} cpu_state_t;

typedef struct {
    uint64_t ds, es, fs, gs;
    uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
    uint64_t error_code, rip, cs, rflags, user_rsp, user_ss;
} cpu_error_state_t;


typedef struct{
    uint64_t tid;
    void* parent;
    uint8_t processor_id;
    thread_state_t thread_state;
    cpu_state_t cpu_state;
} thread_t;

typedef struct {
    uint64_t pid;                 // Process ID
    uint64_t ppid;                // Parent Process ID
    process_state_t process_state;        // Process state
    thread_t threads[MAX_N_THREAD];
    uint8_t n_active_threads;
    uint64_t cr3;
    uint64_t memo_begin;
} pcb_t;

void process_init();

pcb_t* get_default_process();

uint8_t add_new_process(uint64_t pid, uint64_t ppid, uint64_t cr3, uint64_t rip, uint64_t rbp, uint64_t memo_begin);

void remove_process(pcb_t* process);

pcb_t* get_process_pid(uint64_t ppid, uint64_t pid);

void thread_inqueue(thread_t* thread);

thread_t* thread_dequeue();

void thread_remove(thread_t* thread);

uint8_t add_new_thread(pcb_t* process, uint64_t tid, uint64_t rip, uint64_t rbp);

void remove_thread(thread_t* thread);

thread_t* get_thread_tid(pcb_t *process, uint64_t tid);

thread_t* get_process_thread(pcb_t* process, uint8_t n);

void thread_waiting(thread_t* thread);

void process_awake(pcb_t* process);

void thread_awake(thread_t* thread);

uint8_t get_process_index(pcb_t* pcb);