#include <stdint.h>

typedef enum{
	NO_EVENT = 0,
    MOUSE_EVENT = 1,
    KEYBOARD_EVENT = 2
} EventType;

typedef struct {
	char character;
	uint8_t key_code;
	uint8_t key_release;
} KeyInfo;


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
} MouseInfo;

typedef struct{
    EventType type;
    union{
        MouseInfo mouse_info;
        KeyInfo key_info;
    };
} Event;

class EventQueue
{
public:
    EventQueue(uint32_t capacity);
    ~EventQueue();
    uint8_t register_event_queue();
    void wait();
    uint8_t deregister_event_queue();
    Event deque();
private:
    uint32_t capacity;
    uint32_t read_index;
    uint32_t write_index;
    Event* events;
};
