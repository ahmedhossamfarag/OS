#define RESOURCES_N 1
#define RESOURCE_REQUEST_INT 10
#define RESOURCE_FREE_INT 11

#include "scheduler.h"

typedef enum {
    SCREEN
} resource_id_t;

typedef struct{
    resource_id_t id;
    pcb_t* handler;
    uint8_t n_waiters;
    pcb_t* waiters[MAX_N_PROCESS];
} resource_t;

void resources_init();

/* INT 10 , id > eax*/
void resource_request_handler();

void resource_free_handler();