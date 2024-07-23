#define KEYBOARD_DATA_PORT 0x60

#include <stdint.h>

enum Key
{
	NONE,
	ESC,
	CHAR,
	BACKSPACE,
	TAB,
	ENTER,
	CONTROL_LEFT,
	SHIFT_RIGHT,
	NUM_PAD_CHAR,
	ALT_LEFT,
	SPACE,
	CAPS_LOCK,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	NUM_LOCK,
	NUM_PAD_HOME, NUM_PAD_UP, NUM_PAD_PAGE_UP, NUM_PAD_LEFT, NUM_PAD_RIGHT,
	NUM_PAD_END, NUM_PAD_DOWN, NUM_PAD_PAGE_DOWN,
	NUM_PAD_INS, NUM_PAD_DEL, NUM_PAD_ENTER,
	ALT_RIGHT,
	HOME, UP, PAGE_UP, LEFT, RIGHT, END, DOWN, PAGE_DOWN,
	DEL,
};

typedef struct {
	char character;
	uint8_t key_code;
	uint8_t key_release;
} key_info_t;

/* IRQ 1 - INT 0x21*/
void keyboard_handler();

void handle_scancode(uint8_t scancode);

void set_keyboard_handler_proc(void (*proc)(key_info_t));

void apic_keyboard_handler();