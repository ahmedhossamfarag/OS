#include "keyboard.h"
#include "pic.h"
#include "low_level.h"

extern char key_char_map[];
extern char key_code_map[];
extern char key_shift_char_map[];
extern char key_shift_code_map[];
extern char key_extended_char_map[];
extern char key_extended_code_map[];

static void (*keyboard_handler_proc)(KeyInfo);

void keyboard_handler() {

    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    // Handle the scancode
    // (you'll need to implement a scancode-to-keycode mapping)
    handle_scancode(scancode);

    pic_sendEOI(1);
}

static int8_t shift; // Shift key state
static int8_t extended; // Extended key state
static int8_t numlock;

void handle_scancode(uint8_t scancode) {
    KeyInfo info;

    if (scancode == 0xE0) {
        extended = 1;
        return;
    }

    if (scancode > 128) {
        info.key_release = 1;
        scancode -= 128;
    }
    else
    {
        info.key_release = 0;
    }


    // Check if the scancode is a make code (key pressed)
    if (extended == 0) {
        if (shift) {
            info.character = key_shift_char_map[scancode];
            info.key_code = key_shift_code_map[scancode];
        }
        else {
            info.character = key_char_map[scancode];
            info.key_code = key_code_map[scancode];
        }

        if (numlock && info.key_code == NUM_PAD_CHAR) {
            info.character = key_shift_char_map[scancode];
            info.key_code = key_shift_code_map[scancode];
        }
    }
    else {
        // Handle Extended Keys
        info.character = key_extended_char_map[scancode];
        info.key_code = key_extended_code_map[scancode];
        extended = 0;
    }

    if (info.key_code == SHIFT_RIGHT) {
        if (info.key_release)
            shift = 0;
        else
            shift = 1;
    }
    else if (info.key_code == NUM_LOCK && info.key_release) {
        numlock = ~numlock;
    }

    if (keyboard_handler_proc) {
        keyboard_handler_proc(info);
    }
}


void set_keyboard_handler_proc(void(*proc)(KeyInfo))
{
    keyboard_handler_proc = proc;
}
