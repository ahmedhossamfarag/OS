#include "include/screen_print.h"
#include "include/interrupt.h"
#include "include/pic.h"
#include "include/timer.h"

int main () {
    screen_clear();
    screen_print_str("Welcome To Kernel");
    pic_remap();
    init_idt();
    screen_print_str("\nIDT Initialized");
    disable_timer();
    enable_interrupt();
    screen_print_str("\nInterrupts Enabled");
    return 0;
}
