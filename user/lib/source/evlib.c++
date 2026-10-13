#include "evlib.h"
#include "syscall_map.h"
#include "memlib.h"

EventQueue::EventQueue(uint32_t capacity)
{
    this->capacity = capacity;
    this->events = new Event[this->capacity];
    if(!this->events){
        mfree(this, sizeof(EventQueue));
        return;
    }
    this->read_index = 0;
    this->write_index = 0;
}

EventQueue::~EventQueue()
{
    delete[] this->events;
}

uint8_t EventQueue::register_event_queue()
{
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(EVENT_QUEUE_REG_INT), "b"((uint64_t)this));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

void EventQueue::wait()
{
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(EVENT_WAIT_INT));
}

uint8_t EventQueue::deregister_event_queue()
{
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(EVENT_QUEUE_DEREG_INT));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

Event EventQueue::deque()
{
    Event event;
    if(this->read_index == this->write_index){
        event.type = NO_EVENT;
        return event;
    }
    event = this->events[this->read_index];
    this->read_index = (this->read_index + 1) % this->capacity;
    return event;
}
