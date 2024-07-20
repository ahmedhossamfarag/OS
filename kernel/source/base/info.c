#include "info.h"

uint32_t processor_no;
uint32_t memory_size;


uint32_t apic_ids[MAX_N_APIC_IDS];
uint8_t n_apic;

MemoryRegion get_memory_region(uint32_t n)
{
    MemoryRegion* pntr = (MemoryRegion*) MEMORY_MAP + (n*sizeof(MemoryRegion));
    return *pntr;
}

void info_init()
{

    // memory size
    uint32_t* pntr = (uint32_t*) MEMORY_N_MAP;
    uint32_t maps_no = *pntr;
    memory_size = 0;
    MemoryRegion mr;
    for (uint32_t i = 0; i < maps_no; i++)
    {
        mr = get_memory_region(i);
        memory_size += mr.length;
    }

    // apic
    n_apic = 0;
}

uint32_t info_get_processor_no()
{
    return n_apic;
}

uint32_t info_get_memory_size()
{
    return  memory_size;
}


void info_add_apic_id(uint32_t id)
{
    if(n_apic < MAX_N_APIC_IDS){
        apic_ids[n_apic] = id;
        n_apic ++;
    }
}

uint32_t info_get_apic_id(uint8_t i)
{
    return apic_ids[i];
}
