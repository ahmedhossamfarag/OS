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
#include "vga_print.h"

void kernel_load()
{
    #define LOADED 50
    #define TO_LOAD 100
    uint32_t src = 1 + LOADED;
    uint32_t *des = (uint32_t *)(KERNEL_OFFSET + LOADED * 512);
    uint32_t n_sectors = TO_LOAD;
    ata_read(PRIMARY_BASE, 0, src, n_sectors, des);
}

void init()
{
    info_init();
    acpi_init();
    memory_init();
    idt_init();
    pic_init();
    // interrupt_handler_init();
    // gdt_init();
    // pagging_init();
    // // apic_init();
    // scheduler_init();
    // syscall_init();
    vga_init();
    // disk_init();
    // filesystem_init();
}

void setup()
{
    enable_idt();
    enable_interrupt();
    // enable_gdt();
    // // enable_apic();
    // enable_paging();
}

void start()
{
    setup();

    while (1);
}

void ap_setup()
{
    uint32_t *ap_ebp = (uint32_t *)0xA100;
    uint32_t *ap_startup = (uint32_t *)0xA200;

    *ap_startup = (uint32_t)start;

    for (uint8_t apic_id = 1; apic_id < info_get_processor_no(); apic_id++)
    {
        *ap_ebp = KERNEK_STACK_POINTER(apic_id);

        apic_send_init_ipi(apic_id);
        apic_delay(1);
        apic_send_startup_ipi(apic_id, 0xA);
        apic_delay(1);
    }
}

int main()
{
    kernel_load();
    init();
    disable_timer();
    // ap_setup();
    setup();
    println("Welcome To Kernel");
    pci_init();
    
    // load_program();
    // enable_scheduler();

    while (1);

    return 0;
}
