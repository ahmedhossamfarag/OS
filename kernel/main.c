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


extern char _kernel_start[];
extern char _kernel_end[];
static uint8_t flag = 0;

void kernel_shift(){
    if(!flag){
        uint32_t kernel_size = _kernel_end - _kernel_start;
        mem_copy(_kernel_start, (char*)KERNEL_OFFSET, kernel_size);
        flag ++;
        // update esp
        asm("jmp %0"::"r"(KERNEL_OFFSET));
    }
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
    kernel_shift();
    init();
    // ap_setup();
    setup();
    screen_clear();
    println("Welcome To Kernel");


    // load_program();
    // enable_scheduler();

    while (1);

    return 0;
}
