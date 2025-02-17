#pragma once

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

uint8_t queue_remove(queue_t* queue, void* item);

void queue_delete(queue_t* queue, void (*free)(char*,uint32_t));

#pragma endregion

#pragma region Linked List

typedef struct{
    void* data;
    void* next_node;
} list_node_t;

typedef struct{
    list_node_t* head;
    list_node_t* tail;
    char* (*alloc)(uint32_t);
    void (*free)(char*,uint32_t);
} list_t;

list_t* list_new(char* (*alloc)(uint32_t), void (*free)(char*,uint32_t));

void list_append(list_t* list, void* item);

void* list_remove_first(list_t* list);

void list_delete(list_t* list);

#pragma endregion

#pragma region Array

typedef struct{
    uint32_t capacity;
    uint32_t size;
    void** data;
} array_t;

array_t* array_new(uint32_t capacity, char* (*alloc)(uint32_t));

uint8_t array_contains(array_t* arr, void** pntr);

void** array_add(array_t* arr, void* item);

void* array_get(array_t* arr, void** pntr);

uint8_t array_remove(array_t* arr, void** pntr);

void array_delete(array_t* arr, void (*free)(char*,uint32_t));

#pragma endregion

