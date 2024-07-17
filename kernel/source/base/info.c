#include "info.h"

uint32_t processor_no;
uint32_t memory_size;


MemoryRegion get_memory_region(uint32_t n)
{
    MemoryRegion* pntr = (MemoryRegion*) MEMORY_MAP + (n*sizeof(MemoryRegion));
    return *pntr;
}

void info_init()
{
    // processor no
    uint32_t* pntr = (uint32_t*) PROCESSOR_NO;
    processor_no = *pntr;

    // memory size
    pntr = (uint32_t*) MEMORY_N_MAP;
    uint32_t maps_no = *pntr;
    memory_size = 0;
    MemoryRegion mr;
    for (uint32_t i = 0; i < maps_no; i++)
    {
        mr = get_memory_region(i);
        memory_size += mr.length;
    }
}

uint32_t get_processor_no()
{
    return processor_no;
}

uint32_t get_memory_size()
{
    return  memory_size;
}

