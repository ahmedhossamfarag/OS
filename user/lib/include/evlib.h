#include <stdint.h>

typedef enum{
    MOUSE_EVENT = 1,
    KEYBOARD_EVENT = 2
} event_type_t;

typedef struct {
	char character;
	uint8_t key_code;
	uint8_t key_release;
} key_info_t;


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

typedef struct{
    uint8_t left_button;
    uint8_t right_button;
    uint8_t middle_button;
    int16_t delta_x;
    int16_t delta_y;
    uint16_t mouse_x;
    uint16_t mouse_y;
} mouse_info_t;


uint8_t event_register(uint8_t type, void (*handler)(), void* args);

void event_wait();