#include "pages.h"
#include "memory.h"
#include "info.h"

static uint32_t segment_no;
static uint8_t* pg_arr;

static void pages_alloc_kernel(){
    pg_arr[0] = 0;

    for (uint32_t i = KERNEL_START / SEGMENT_SIZE; i <= KERNEL_END / SEGMENT_SIZE; i++)
    {
        pg_arr[i] = 0;
    }
    
    graphics_info_t* g = info_get_graphics();

    for (uint32_t i = (g->FameBufferBase / SEGMENT_SIZE); i <= (g->FameBufferBase + g->FrameBufferSize) / SEGMENT_SIZE; i++)
    {
        pg_arr[i] = 0;
    }
    
    for (uint32_t i = DRIVERS_OFFSET / SEGMENT_SIZE; i < segment_no; i++)
    {
        pg_arr[i] = 0;
    }

    for (uint32_t i = info_get_memory_size() / SEGMENT_SIZE; i < segment_no; i++)
    {
        pg_arr[i] = 0;
    }
    
}

void pages_init(){
    segment_no = info_get_memory_size() / SEGMENT_SIZE;

    pg_arr = (uint8_t*) alloc(segment_no);

    for (uint32_t i = 0; i < segment_no; i++)
    {
        pg_arr[i] = 1;
    }

    pages_alloc_kernel();
}

uint64_t pages_alloc(){
    for (uint32_t i = 0; i < segment_no; i++)
    {
        if(pg_arr[i]){
            pg_arr[i] = 0;
            return (uint64_t)i * SEGMENT_SIZE;
        }
    }
    return 0;
}

uint64_t pages_alloc_next(uint64_t seg){
    seg /= SEGMENT_SIZE;
    for (uint32_t i = seg; i < segment_no; i++)
    {
        if(pg_arr[i]){
            pg_arr[i] = 0;
            return (uint64_t)i * SEGMENT_SIZE;
        }
    }
    return 0;
}

void pages_free(uint64_t seg){
    seg /= SEGMENT_SIZE;
    if(seg > 0 && seg < segment_no){
        pg_arr[seg] = 1;
    }
}

uint32_t pages_nfree_segments(){
    uint32_t n = 0;
    for (uint32_t i = 0; i < segment_no; i++)
    {
        n += pg_arr[i];
    }
    return n;
}