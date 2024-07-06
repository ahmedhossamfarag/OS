#include "screen_print.h"
#include "interrupt.h"
#include "pic.h"
#include "timer.h"
#include "memory.h"
#include "libc.h"

int main () {
    screen_clear();
    screen_print_str("Welcome To Kernel");
    pic_remap();
    init_idt();
    screen_print_str("\nIDT Initialized");
    disable_timer();
    enable_interrupt();
    screen_print_str("\nInterrupts Enabled");
    memory_init();
    screen_print_str("\nMemory In");
    return 0;
}
