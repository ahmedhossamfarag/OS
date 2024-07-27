#include "dslib.h"

#pragma region Queue

queue_t *queue_new(uint32_t capacity, char *(*alloc)(uint32_t))
{
    queue_t* queue = (queue_t*) alloc(sizeof(queue_t));
    if(!queue){
        return 0;
    }
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

void queue_delete(queue_t *queue, void (*free)(char *, uint32_t))
{
    free((char*)queue->data, queue->capacity * sizeof(void*));
    free((char*)queue, sizeof(queue_t));
}


#pragma endregion

#pragma region Linked List

list_t *list_new(char *(*alloc)(uint32_t), void (*free)(char *, uint32_t))
{
    list_t* list = (list_t*) alloc(sizeof(list_t));
    if(!list){
        return 0;
    }
    list->head = list->tail = 0;
    list->alloc = alloc;
    list->free = free;
    return list;
}

void list_append(list_t *list, void *item)
{
    list_node_t* node = (list_node_t*) list->alloc(sizeof(list_node_t));

    if(node){
        node->next_node = 0;
        node->data = item;

        if(list->tail){
            list->tail->next_node = node;
            list->tail = node;
        }else{
            list->head = list->tail = node;
        }
    }
}

void *list_remove_first(list_t *list)
{
    if(list->head){
        void* item = list->head->data;
        list_node_t* next = list->head->next_node;
        list->free((char*)list->head, sizeof(list_node_t));
        list->head = next;
        if(!next){
            list->tail = 0;
        }
        return item;
    }
    return 0;
}

void list_delete(list_t *list)
{
    void (*free)(char *, uint32_t) = list->free;
    list_node_t* node = list->head;
    while(node){
        list_node_t* next = node->next_node;
        free((char*)node, sizeof(list_node_t));
        node = next;
    }
    free((char*)list, sizeof(list_t));
}

#pragma endregion

#pragma region Array

array_t* array_new(uint32_t capacity, char* (*alloc)(uint32_t)){
    array_t* arr = (array_t*) alloc(sizeof(array_t));
    if(!arr){
        return 0;
    }

    arr->capacity = capacity;
    arr->size = 0;
    arr->data = (void**) alloc(capacity*sizeof(void*));

    return arr;
}

uint8_t array_contains(array_t* arr, void** pntr){
    for (void** p = arr->data; p < arr->data+arr->size; p++)
    {
        if(p == pntr){
            return 1;
        }
    }
    return 0;
}

void** array_add(array_t* arr, void* item){
    if(arr->size < arr->capacity){
        void** pntr = arr->data + arr->size;
        *pntr = item;
        arr->size ++;
        return pntr;
    }
    return 0;
}

void* array_get(array_t* arr, void** pntr){
    if(array_contains(arr, pntr)){
        return *pntr;
    }
    return 0;
}

uint8_t array_remove(array_t* arr, void** pntr){
    if(array_contains(arr, pntr)){
        for (void** p = pntr; p < arr->data + arr->size - 1; p++)
        {
            *p = *(p+1);
        }
        arr->size --;
        return 1;
    }
    return 0;
}

void array_delete(array_t* arr, void (*free)(char*,uint32_t)){
    free((char*)arr->data, arr->capacity * sizeof(void*));
    free((char*)arr, sizeof(array_t));
}

#pragma endregion