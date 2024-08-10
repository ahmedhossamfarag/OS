#include "screen_print.h"
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
#include "pagging.h"
#include "interrupt_handler.h"
#include "loader.h"
#include "apic.h"
#include "acpi.h"
#include "info.h"
#include "ata.h"
#include "syscall.h"
#include "pci.h"
#include "vga.h"
// #include "vga_print.h"

void init()
{
    info_init();
    acpi_init();
    memory_init();
    idt_init();
    pic_init();
    interrupt_handler_init();
    gdt_init();
    pagging_init();
    apic_init();
    scheduler_init();
    syscall_init();
    // vga_init();
    // disk_init();
    // filesystem_init();
}

void setup()
{
    enable_idt();
    enable_interrupt();
    enable_gdt();
    enable_apic();
    enable_paging();
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
    uint8_t* code_offset = (uint8_t*)0xA000;
    uint32_t* stack_size_pntr = (uint32_t*)0xA100;
    uint32_t* start_pntr = (uint32_t*)0xA200;

    mem_copy((char*)_ap_setup_start, (char*)code_offset, _ap_setup_end - _ap_setup_start);

    *start_pntr = (uint32_t) ap_start;

    for (uint8_t apic_id = 1; apic_id < info_get_processor_no(); apic_id++)
    {
        *stack_size_pntr = KERNEK_STACK_POINTER(apic_id);

        apic_send_init_ipi(apic_id);
        apic_delay(1);
        apic_send_startup_ipi(apic_id, (uint32_t)code_offset/0x1000);
        apic_delay(1);
    }
}

int kernel_main()
{
    init();
    setup();
    ap_setup();
    screen_clear();
    println("Welcome To Kernel");


    // load_program();
    // enable_scheduler();

    while (1);

    return 0;
}
