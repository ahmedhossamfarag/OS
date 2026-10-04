#include "evlib.h"
#include "syscall_map.h"

uint8_t event_register(uint8_t type, void (*handler)(), void* args){
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(EVENT_REG_INT), "a"(type), "b"((uint64_t)handler),"d"(args));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

void event_wait(){
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(EVENT_WAIT_INT));
}

uint8_t event_deregister(uint8_t type){
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(EVENT_DEREG_INT), "a"(type));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}