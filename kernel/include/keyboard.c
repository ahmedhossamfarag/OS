#include "keyboard.h"
#include "pic.h"
#include "low_level.h"
#include "screen_print.h"

void keyboard_handler() {
    
    screen_print_str("IRQ1");

    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    // Handle the scancode
    // (you'll need to implement a scancode-to-keycode mapping)
    handle_scancode(scancode);

    pic_sendEOI(1);
}

void handle_scancode(uint8_t scancode) {
    // Simple example for handling scancodes
    // You'll need to expand this for full functionality
    if (scancode & 0x80) {
        // Key released
    } else {
        // Key pressed
        // Convert scancode to keycode or ASCII
    }
}
