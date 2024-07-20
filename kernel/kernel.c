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
#include "resources.h"
#include "libc.h"
#include "pagging.h"
#include "interrupt_handler.h"
#include "loader.h"
#include "apic.h"
#include "acpi.h"

int main () {
    screen_clear();
    screen_print_str("Welcome To Kernel");
    acpi_init();
    screen_print_str("\nACPI In");
    init_idt();
    pic_init();
    screen_print_str("\nIDT Initialized");
    disable_timer();
    enable_interrupt();
    interrupt_handler_init();
    screen_print_str("\nInterrupts Enabled");
    memory_init();
    screen_print_str("\nMemory In");
    init_gdt();
    screen_print_str("\nGDT & TSS In");
    init_page_tables();
    screen_print_str("\nPaging In\n");

    apic_init();

    while (1);
    
    return 0;
}
