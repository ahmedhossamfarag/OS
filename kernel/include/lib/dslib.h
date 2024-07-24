#include <stdint.h>

#pragma region Queue

typedef struct{
    uint32_t capacity;
    uint32_t size;
    uint32_t head;
    uint32_t tail;
    void** data;
} queue_t;

queue_t* queue_new(uint32_t capacity, char* (*alloc)(uint32_t));

void queue_inque(queue_t* queue, void* item);

void* queue_deque(queue_t* queue);

void queue_remove(queue_t* queue, void* item);

#pragma endregion

