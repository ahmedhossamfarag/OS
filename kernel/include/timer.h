#define TIMER_COMMAND 0x43
#define TIMER_DATA 0x40
#define TIMER_ENABLED 0x36
#define TIMER_DISABED 0x30

#include <stdint.h>

void enable_timer(uint32_t frequency);

void disable_timer();

/* IRQ 0 - INT 0x20*/
void timer_handler(void);
