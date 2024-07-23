#define RESOURCES_N 1

#include "scheduler.h"

typedef enum {
    SCREEN
} resource_id_t;

typedef struct{
    resource_id_t id;
    thread_t* handler;
    uint8_t n_waiters;
    thread_t* waiters[MAX_N_PROCESS*MAX_N_THREAD];
} resource_t;

void resources_init();

/* INT 10 , id > eax*/
void resource_request_handler();

void resource_free_handler();