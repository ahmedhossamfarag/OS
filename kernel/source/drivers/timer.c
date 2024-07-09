#include "timer.h"
#include "low_level.h"
#include "pic.h"



static void (*timer_handler_proc)();

void enable_timer(uint32_t frequency) {
    uint32_t divisor = CLOCK_FREQ / frequency;
    outb(TIMER_COMMAND, TIMER_ENABLED); // Command byte for PIT channel 0
    outb(TIMER_DATA, divisor & 0xFF); // Low byte of divisor
    outb(TIMER_DATA, (divisor >> 8) & 0xFF); // High byte of divisor
}

void disable_timer() {
    // Send command byte to PIT to stop the timer
    outb(TIMER_COMMAND, TIMER_DISABED); // Command byte for PIT channel 0
    outb(TIMER_DATA, 0);    // Low byte of divisor
    outb(TIMER_DATA, 0);    // High byte of divisor
}

void timer_handler(void) {
    if (timer_handler_proc) {
        timer_handler_proc();
    }
    pic_sendEOI(0);
}

void set_timer_handler_proc(void (*proc)())
{
    timer_handler_proc = proc;
}
