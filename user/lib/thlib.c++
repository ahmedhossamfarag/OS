#include "thlib.h"
#include "syscall_map.h"

uint8_t process_create(uint64_t pid, FILE* file){
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(CREATE_PROCESS_SYSCALL), "a"(pid), "b"((uint64_t)file));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t thread_create(uint64_t tid, void* start, uint64_t stack){
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(CREATE_THREAD_SYSCALL), "a"(tid), "b"((uint64_t)start), "d"(stack));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

void process_exit(){
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(EXIT_PROCESS_SYSCALL));
}

void thread_exit(){
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(EXIT_THREAD_SYSCALL));
}

uint8_t process_terminate(uint64_t pid){
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(TERMINATE_PROCESS_SYSCALL), "a"(pid));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t thread_terminate(uint64_t tid){
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(TERMINATE_THREAD_SYSCALL), "a"(tid));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}
