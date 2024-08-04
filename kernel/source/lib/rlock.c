#include "rlock.h"

void resource_lock_request(void **lock, void *handler)
{
    do{
        while(*lock);
        *lock = handler;
    }while(*lock != handler);
}

void resource_lock_free(void** lock, void* handler){
    if(*lock == handler){
        *lock = 0;
    }
}