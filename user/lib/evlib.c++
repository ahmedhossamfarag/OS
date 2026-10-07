#include "evlib.h"
#include "syscall_map.h"
#include "memlib.h"

event_queue_t* event_queue_new(uint32_t capacity){
    event_queue_t* queue = (event_queue_t*) malloc(sizeof(event_queue_t));
    if(!queue){
        return 0;
    }
    queue->capacity = capacity;
    queue->events = (event_t*) malloc(capacity * sizeof(event_t));
    if(!queue->events){
        mfree(queue, sizeof(event_queue_t));
        return 0;
    }
    queue->read_index = 0;
    queue->write_index = 0;
    return queue;
}

uint8_t event_queue_register(event_queue_t* queue){
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(EVENT_QUEUE_REG_INT), "b"((uint64_t)queue));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

void event_wait(){
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(EVENT_WAIT_INT));
}

uint8_t event_queue_deregister(){
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(EVENT_QUEUE_DEREG_INT));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

event_t event_queue_deque(event_queue_t *queue)
{
    event_t event;
    if(queue->read_index == queue->write_index){
        event.type = NO_EVENT;
        return event;
    }
    event = queue->events[queue->read_index];
    queue->read_index = (queue->read_index + 1) % queue->capacity;
    return event;
}
