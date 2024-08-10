#include "info.h"

// #define MEMORY_N_MAP 0x8000
// #define MEMORY_MAP 0x8004
// #define DISk_SIZE 0x8400

#define MEMORY_DEFAULT_SIZE 0x100000000 // 4GiB
#define MAX_N_APIC_IDS 10

extern uint32_t grub_magic;
extern uint32_t boot_info;

uint32_t processor_no;
uint64_t memory_size;


uint32_t apic_ids[MAX_N_APIC_IDS];
uint8_t n_apic;

void info_init()
{
    if(grub_magic == MULTIBOOT_MAGIC){
        multiboot_info_t* m = (multiboot_info_t*) boot_info;
        memory_size = (m->mem_lower + m->mem_upper + 1024) * 1024 ;
    }else{
        memory_size = MEMORY_DEFAULT_SIZE;
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