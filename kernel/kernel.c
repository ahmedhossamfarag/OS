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

void proc1(){
    while (1)
    {
        print_str("Process_1;");
    }
}

void proc2(){
    while (1)
    {
        print_str("Process_2;");
    }
}

int main () {
    screen_clear();
    screen_print_str("Welcome To Kernel");
    init_idt();
    pic_init();
    screen_print_str("\nIDT Initialized");
    disable_timer();
    enable_interrupt();
    screen_print_str("\nInterrupts Enabled");
    memory_init();
    screen_print_str("\nMemory In");
    init_gdt();
    screen_print_str("\nGDT & TSS In\n");
    resources_init();
    scheduler_init();
    add_new_process(1, 0, (uint32_t) proc1, 0x98000);
    add_new_process(2, 0, (uint32_t) proc2, 0xA0000);
    scheduler_enable();

    while (1)
    {
    }
    
    return 0;
}
