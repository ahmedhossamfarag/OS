#include "thlib.h"
#include "syscall_map.h"

uint8_t process_create(uint32_t pid, FILE* file, uint32_t stack){
    asm("mov %0, %%esi\n\t""int $0x80"::"i"(CREATE_PROCESS_SYSCALL), "a"(pid), "b"((uint32_t)file), "d"(stack));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t thread_create(uint32_t tid, void* start, uint32_t stack){
    asm("mov %0, %%esi\n\t""int $0x80"::"i"(CREATE_THREAD_SYSCALL), "a"(tid), "b"((uint32_t)start), "d"(stack));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));
    
    return (uint8_t)result;
}

void process_exit(){
    asm("mov %0, %%esi\n\t""int $0x80"::"i"(EXIT_PROCESS_SYSCALL));
}

void thread_exit(){
    asm("mov %0, %%esi\n\t""int $0x80"::"i"(EXIT_THREAD_SYSCALL));
}

uint8_t process_terminate(uint32_t pid){
    asm("mov %0, %%esi\n\t""int $0x80"::"i"(TERMINATE_PROCESS_SYSCALL), "a"(pid));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t thread_terminate(uint32_t tid){
    asm("mov %0, %%esi\n\t""int $0x80"::"i"(TERMINATE_THREAD_SYSCALL), "a"(tid));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));
    
    return (uint8_t)result;
}
