#define KEYBOARD_DATA_PORT 0x60

#include <stdint.h>

/* IRQ 1 - INT 0x21*/
void keyboard_handler();

void handle_scancode(uint8_t scancode);
