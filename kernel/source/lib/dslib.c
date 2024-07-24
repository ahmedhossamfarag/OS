#include "dslib.h"

#pragma region Queue

queue_t *queue_new(uint32_t capacity, char *(*alloc)(uint32_t))
{
    queue_t* queue = (queue_t*) alloc(sizeof(queue_t));
    queue->capacity = capacity;
    queue->data = (void**) alloc(capacity * sizeof(void*));
    queue->size = queue->head = queue->tail = 0;
    return queue;
}

void queue_inque(queue_t *queue, void *item)
{
    if(queue->size < queue->capacity){
        if(queue->size == 0){
            queue->data[0] = item;
            queue->head = queue->tail = 0;
        }else{
            queue->tail = (queue->tail + 1) % queue->capacity;
            queue->data[queue->tail] = item;
        }
        queue->size ++;
    }
}

void *queue_deque(queue_t *queue)
{
    if(queue->size){
        void *item = queue->data[queue->head];
        queue->head = (queue->head + 1) % queue->capacity;
        queue->size --;
        return item;
    }
    return 0;
}

void queue_remove(queue_t *queue, void *item)
{
    if(!queue->size) return;

    uint32_t queue_end = queue->tail >= queue->head ? queue->tail : queue->tail + queue->capacity;
    for (uint32_t i = queue->head; i <= queue_end; i++)
    {
        if(queue->data[i%queue->capacity] == item){
            for (uint32_t j = i; j < queue_end - 1; j++)
            {
                queue->data[j%queue->capacity] = queue->data[(j+1)%queue->capacity];
            }
            queue->size --;
            queue->tail = (queue_end - 1) % queue->capacity;
            break;
        }
    }
    
}

#pragma endregion