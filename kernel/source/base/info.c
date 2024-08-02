#include "info.h"

uint32_t processor_no;
uint32_t memory_size;


uint32_t apic_ids[MAX_N_APIC_IDS];
uint8_t n_apic;

memory_region_t get_memory_region(uint32_t n)
{
    memory_region_t* pntr = (memory_region_t*) MEMORY_MAP + (n*sizeof(memory_region_t));
    return *pntr;
}

void info_init()
{

    // memory size
    uint32_t* pntr = (uint32_t*) MEMORY_N_MAP;
    uint32_t maps_no = *pntr;
    memory_size = 0;
    memory_region_t mr;
    for (uint32_t i = 0; i < maps_no; i++)
    {
        mr = get_memory_region(i);
        memory_size += mr.length;
    }

    // apic
    n_apic = 0;

    vesa_init();
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



uint8_t info_get_processor_id()
{
    unsigned int eax, ebx, ecx, edx;
    
    // Call CPUID with EAX=1
    asm volatile (
        "cpuid"
        : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
        : "a" (1)
    );
    
    unsigned int processor_id = (ebx >> 24) & 0xFF;
    return processor_id;
}

#pragma region VESA

uint16_t vesa_mode;
vbe_mode_info_t mode_info;

void vesa_init(){
    uint16_t* mode_pntr = (uint16_t*) VESA_MODE;
    vesa_mode = *mode_pntr;
    if(vesa_mode != 0xFFFF){
        vbe_mode_info_t* info_pntr = (vbe_mode_info_t*) VESA_MODE_INFO;
        mode_info = *info_pntr;
    }
}

uint16_t vesa_get_mode(){
    return vesa_mode;
}

vbe_mode_info_t* vesa_get_mode_info(){
    return &mode_info;
}

#pragma endregion