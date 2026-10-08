// #include "screen_print.h"
#include "interrupt.h"
#include "pic.h"
#include "timer.h"
#include "memory.h"
#include "strlib.h"
#include "disk.h"
#include "file_system.h"
#include "scheduler.h"
#include "gdt.h"
#include "tss.h"
#include "resources.h"
#include "libc.h"
#include "paging.h"
#include "interrupt_handler.h"
#include "loader.h"
#include "apic.h"
#include "acpi.h"
#include "info.h"
#include "ata.h"
#include "syscall.h"
#include "pci.h"
#include "vga.h"
#include "vga_print.h"
#include "graphics.h"
#include "windows.h"
#include "mouse.h"

void init()
{
    info_init();
    vga_init();
    acpi_init();
    memory_init();
    idt_init();
    pic_init();
    interrupt_handler_init();
    gdt_init();
    paging_init();
    apic_init();
    scheduler_init();
    syscall_init();
    ata_init();
    disk_init();
    filesystem_init();
    graphics_init();
    windows_init();
    mouse_init();
}

/**
 * Jump to virtual kernel address
 */
__attribute__((noinline))
void jump_to_virtual_kerenl_address()
{
    asm (
        "pop %%rax\n\t"
        "movabs %0, %%rcx\n\t"
        "add %%rcx, %%rax\n\t"
        "add %%rcx, %%rsp\n\t"
        "push %%rax\n\t"
        "ret\n\t"
        :
        : "i"(MEMORY_VIRTUAL_START)
        : "rax", "rcx", "memory"
    );
}

void setup()
{
    disable_interrupt();
    enable_paging();
    enable_gdt();
    enable_idt();
    enable_apic();
    enable_interrupt();
}

void ap_start()
{
    setup();

    while (1);
}

extern uint8_t _ap_setup_start[];
extern uint8_t _ap_setup_end[];

void ap_setup()
{
    extern uint64_t* default_paging;

    uint8_t* code_offset = (uint8_t*)0xA000;
    uint64_t* stack_size_pntr = (uint64_t*)0xA100;
    uint64_t* start_pntr = (uint64_t*)0xA200;
    uint32_t* dir_pntr = (uint32_t*)0xA300;

    mem_copy((char*)_ap_setup_start, (char*)code_offset, _ap_setup_end - _ap_setup_start);

    *start_pntr = (uint64_t) ap_start;
    *dir_pntr = (uint64_t) default_paging & 0xFFFFFFFF;

    for (uint8_t apic_id = 1; apic_id < info_get_processor_no(); apic_id++)
    {
        *stack_size_pntr = KERNEK_STACK_POINTER(apic_id);

        apic_send_init_ipi(apic_id);
        apic_delay(1);
        apic_send_startup_ipi(apic_id, (uint64_t)code_offset/0x1000);
        apic_delay(1);
    }
}

static void loader_success(){
    println("Loader Success");
    apic_delay(1);
    graphics_clear(0);
    graphics_update();
    apic_delay(1);
    enable_scheduler();
}

static void loader_error(){
    println("Failed To Load Program");
} 


int kernel_main()
{
    init();
    setup();
    jump_to_virtual_kerenl_address();
    ap_setup();
    vga_print_clear(0);
    println("Welcome To kernel");
    
    load_program(loader_success, loader_error);

    while (1);

    return 0;
}
