#include "keyboard.h"

static char key_char_map[] = { 
    0, 0,
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    0, 0,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    0, 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0,
    '*',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static char key_shift_char_map[] = { 
    0, 0,
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
    0, 0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
    0, 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0,
    '*',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static char key_extended_char_map[] = { 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, '/', 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static enum Key key_code_map[] = {
    NONE, ESC,
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR,
    BACKSPACE, TAB,
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR,
    ENTER, CONTROL_LEFT,
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR,
    NONE,
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR,
    SHIFT_RIGHT,
    NUM_PAD_CHAR,
    ALT_LEFT, SPACE, CAPS_LOCK, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, NUM_LOCK, NONE,
    NUM_PAD_CHAR, NUM_PAD_CHAR, NUM_PAD_CHAR, NUM_PAD_CHAR, NUM_PAD_CHAR, NUM_PAD_CHAR, NUM_PAD_CHAR, NUM_PAD_CHAR, NUM_PAD_CHAR, NUM_PAD_CHAR, NUM_PAD_CHAR, NUM_PAD_CHAR, NUM_PAD_CHAR,
    NONE, NONE, NONE, F11, F12, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE
};

static enum Key key_shift_code_map[] = {
    NONE, ESC,
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR,
    BACKSPACE, TAB,
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR,
    ENTER, CONTROL_LEFT,
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR,
    NONE,
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR,
    SHIFT_RIGHT,
    NUM_PAD_CHAR,
    ALT_LEFT, SPACE, CAPS_LOCK, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, NUM_LOCK, NONE,
    NUM_PAD_HOME, NUM_PAD_UP, NUM_PAD_PAGE_UP, NUM_PAD_CHAR, NUM_PAD_LEFT, NONE, NUM_PAD_RIGHT, NUM_PAD_CHAR, NUM_PAD_END, NUM_PAD_DOWN, NUM_PAD_PAGE_DOWN, NUM_PAD_INS, NUM_PAD_DEL,
    NONE, NONE, NONE, F11, F12, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE
};

static enum Key key_extended_code_map[] = {
    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    NONE, NONE, NONE, NONE, NUM_PAD_ENTER, NONE, NONE, NONE, NONE, NONE, NONE,
    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    NONE, NONE, NONE, NONE, NONE, NONE, NUM_PAD_CHAR, NONE, NONE, ALT_RIGHT,
    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    NONE, NONE, HOME, UP, PAGE_UP, NONE, LEFT, NONE, RIGHT, NONE, END, DOWN,
    PAGE_DOWN, NONE, DEL, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE
};

