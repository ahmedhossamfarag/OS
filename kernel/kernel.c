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
#include "info.h"
#include "ata.h"

void kernel_load(){
    uint32_t src = 1 + 50;
    uint32_t* des = (uint32_t*) (0xA000000 + 50 * 512);
    uint32_t n_sectors = 50;
    ata_read(PRIMARY_BASE, 0, src, n_sectors, des);
}

void init(){
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
}

void setup(){
    enable_idt();
    enable_interrupt();
    enable_gdt();
    enable_paging();
}

int main () {
    kernel_load();
    screen_clear();
    init();
    setup();
    screen_print_str("Welcome To Kernel");

    while (1);
    
    return 0;
}
