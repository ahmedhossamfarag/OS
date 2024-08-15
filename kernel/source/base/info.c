#include "info.h"

// #define MEMORY_N_MAP 0x8000
// #define MEMORY_MAP 0x8004
// #define DISk_SIZE 0x8400

#define MEMORY_DEFAULT_SIZE 0x100000000 // 4GiB

extern uint32_t bl_magic;
extern uint32_t boot_info_ptr;
extern uint32_t memory_info_ptr;
extern uint32_t graphics_info_ptr;

uint32_t processor_no;
memory_info_t memory_info;
graphics_info_t graphics_info;

static void info_mbl_init(){
    memory_info_t* mptr = (memory_info_t*) memory_info_ptr;
    memory_info = *mptr;

    graphics_info_t* gptr = (graphics_info_t*) graphics_info_ptr;
    graphics_info = *gptr;
}

static void info_bios_init(){
    memory_info.MomorySizeInMB = MEMORY_DEFAULT_SIZE / 1024 / 1024;
    memory_info.RSDP = 0;

    uint16_t* mode_pntr = (uint16_t*) VESA_MODE;
    uint16_t vesa_mode = *mode_pntr;
    if(vesa_mode != 0xFFFF){
        vbe_mode_info_t* info_pntr = (vbe_mode_info_t*) VESA_MODE_INFO;
        graphics_info.FameBufferBase = info_pntr->framebuffer;
        graphics_info.Width = info_pntr->width;
        graphics_info.Height = info_pntr->height;
        graphics_info.FrameBufferSize = info_pntr->pitch * info_pntr->height;
        graphics_info.PixelsPerScanLine = info_pntr->width;
        graphics_info.RedMask = info_pntr->red_mask;
        graphics_info.GreenMask = info_pntr->green_mask;
        graphics_info.BlueMask = info_pntr->blue_mask;
        graphics_info.PixelFormat = 0;
    }else{
        graphics_info.PixelFormat = 0xFFFF;
    }
}

void info_init()
{
    if(bl_magic == MYBOOTLOADER_MAGIC){
        info_mbl_init();
    }else{
        info_bios_init();
    }

    processor_no = 0;
}

uint32_t info_get_processor_no()
{
    return processor_no ? processor_no : 1;
}

uint32_t info_get_memory_size()
{
    return  memory_info.MomorySizeInMB * 1024 * 1024;
}


void info_add_apic_id()
{
    processor_no ++;
}

uint32_t info_get_apic_id(uint8_t i)
{
    return i;
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

graphics_info_t* info_get_graphics(){
    return &graphics_info;
}

uint32_t info_get_rsdp(){
    return memory_info.RSDP;
}

uint8_t info_is_gpt()
{
    return bl_magic == MYBOOTLOADER_MAGIC;
}
