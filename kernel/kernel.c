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

void kernel_load()
{
    uint32_t src = 1 + 50;
    uint32_t *des = (uint32_t *)(0xA000000 + 50 * 512);
    uint32_t n_sectors = 50;
    ata_read(PRIMARY_BASE, 0, src, n_sectors, des);
}

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
    resources_init();
    disk_init();
}

void setup()
{
    enable_idt();
    enable_interrupt();
    enable_gdt();
    enable_apic();
    enable_paging();
    enable_scheduler();
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

void succ(){
    print("succ");
}

void error(){
    print("error");
}

extern disk_args_t disk_args;

void f2(){
    println("closed");
}

void f1(){
    println("open");
    char s[10];
    println(int_to_str(disk_alloc(1), s));
    println(int_to_str(disk_alloc(1), s));
    disk_free(102, 1);
    disk_free(103, 1);
    println(int_to_str(disk_alloc(2), s));
    println(int_to_str(disk_alloc(1), s));
    disk_close(f2, error);
}

int main()
{
    kernel_load();
    init();
    // ap_setup();
    setup();
    screen_clear();
    print("Welcome To Kernel\n");
    // load_program();

    disk_open(f1, error);

    while (1);

    return 0;
}
