#include "iolib.h"
#include "thlib.h"
#include "memlib.h"
#include "strlib.h"

void thread1(){
    prints("thread1");
    thread_exit();
}

void thread2(){
    prints("thread2");
    while(1);
}
extern uint32_t mhead;
void main(){
    prints("shell..\n");
    
    minit(); // Initialize memory

    #define stack (uint32_t) malloc(0x200) + 0x200

    thread_create(2, thread1, stack);
    thread_create(3, thread2, stack);

    while(1);
}

