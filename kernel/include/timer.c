#include "timer.h"
#include "low_level.h"
#include "pic.h"
#include "screen_print.h"

void enable_timer(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;
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
    screen_print_str("IRQ0");
    pic_sendEOI(0);
}