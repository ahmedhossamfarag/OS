#include "io_syscall.h"
#include "resources.h"
#include "memory.h"

extern resource_queue_t* keyboard_queue;
extern resource_queue_t* disk_queue;

void io_syscall_init(){
    keyboard_queue = resource_queue_new(KEYBOARD, QUEUE_CAPACITY);
    disk_queue = resource_queue_new(DISK, QUEUE_CAPACITY);
}

void syscall_init(){
    io_syscall_init();
}
