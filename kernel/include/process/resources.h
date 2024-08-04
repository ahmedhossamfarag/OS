#define RESOURCES_N 3

#include "scheduler.h"
#include "dslib.h"

typedef enum {
    SCREEN,
    KEYBOARD,
    DISK
} resource_id_t;

typedef struct
{
    thread_t* thread;
    void(*handler_proc)();
} resource_request_t;

typedef struct
{
    resource_id_t id;
    queue_t* queue;
    thread_t* handler;
    void* lock;
}resource_queue_t;


resource_queue_t* resource_queue_new(resource_id_t id, uint32_t capacity);

void resource_queue_inque(resource_queue_t* rq, thread_t* thread, void(*proc)());

void resource_queue_deque(resource_queue_t* rq);

#include "rlock.h"