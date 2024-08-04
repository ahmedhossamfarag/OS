#include "pages.h"
#include "memory.h"
#include "info.h"

static uint8_t* pg_arr;

static void pages_alloc_kernel(){
    pg_arr[0] = 0;

    for (int i = KERNEL_OFFSET / SEGMENT_SIZE; i < KERNEL_END / SEGMENT_SIZE; i++)
    {
        pg_arr[i] = 0;
    }
    
    for (int i = DRIVERS_OFFSET / SEGMENT_SIZE; i < SEGMENT_NO; i++)
    {
        pg_arr[i] = 0;
    }
    
}

void pages_init(){
    pg_arr = (uint8_t*) alloc(SEGMENT_NO);

    for (int i = 0; i < SEGMENT_NO; i++)
    {
        pg_arr[i] = 1;
    }

    pages_alloc_kernel();
}

uint32_t pages_alloc(){
    for (int i = 0; i < SEGMENT_NO; i++)
    {
        if(pg_arr[i]){
            pg_arr[i] = 0;
            return i * SEGMENT_SIZE;
        }
    }
    return 0;
}

uint32_t pages_alloc_next(uint32_t seg){
    seg /= SEGMENT_SIZE;
    for (int i = seg; i < SEGMENT_NO; i++)
    {
        if(pg_arr[i]){
            pg_arr[i] = 0;
            return i * SEGMENT_SIZE;
        }
    }
    return 0;
}

void pages_free(uint32_t seg){
    seg /= SEGMENT_SIZE;
    if(seg > 0 && seg < SEGMENT_NO){
        pg_arr[seg] = 1;
    }
}

uint32_t pages_nfree_segments(){
    uint32_t n = 0;
    for (int i = 0; i < SEGMENT_NO; i++)
    {
        n += pg_arr[i];
    }
    return n;
}