.section rodata

.global key_char_map
.global key_shift_char_map
.global key_extended_char_map
.global key_code_map
.global key_shift_code_map
.global key_extended_code_map

.set NONE,  0
.set ESC_,  1
.set CHAR,  2
.set BACKSPACE,  3
.set TAB,  4
.set ENTER_,  5
.set CONTROL_LEFT,  6
.set SHIFT_RIGHT,  7
.set NUM_PAD_CHAR,  8
.set ALT_LEFT,  9
.set SPACE,  10
.set CAPS_LOCK,  11
.set F1,  12
.set F2,  13
.set F3,  14
.set F4,  15
.set F5,  16
.set F6,  17
.set F7,  18
.set F8,  19
.set F9,  20
.set F10,  21
.set F11,  22
.set F12,  23
.set NUM_LOCK,  24
.set NUM_PAD_HOME,  25
.set NUM_PAD_UP,  26
.set NUM_PAD_PAGE_UP,  27
.set NUM_PAD_LEFT,  28
.set NUM_PAD_RIGHT,  29
.set NUM_PAD_END,  30
.set NUM_PAD_DOWN,  31
.set NUM_PAD_PAGE_DOWN,  32
.set NUM_PAD_INS,  33
.set NUM_PAD_DEL,  34
.set NUM_PAD_ENTER,  35
.set ALT_RIGHT,  36
.set HOME,  37
.set UP,  38
.set PAGE_UP,  39
.set LEFT,  40
.set RIGHT,  41
.set END,  42
.set DOWN,  43
.set PAGE_DOWN,  44
.set DEL,  45


key_char_map:
    .skip 2
    .byte '1'
    .byte '2'
    .byte '3'
    .byte '4'
    .byte '5'
    .byte '6'
    .byte '7'
    .byte '8'
    .byte '9'
    .byte '0'
    .byte '-'
    .byte '='
    .skip 2
    .byte 'q'
    .byte 'w'
    .byte 'e'
    .byte 'r'
    .byte 't'
    .byte 'y'
    .byte 'u'
    .byte 'i'
    .byte 'o'
    .byte 'p'
    .byte '['
    .byte ']'
    .skip 2
    .byte 'a'
    .byte 's'
    .byte 'd'
    .byte 'f'
    .byte 'g'
    .byte 'h'
    .byte 'j'
    .byte 'k'
    .byte 'l'
    .byte ';'
    .byte '''
    .byte '`'
    .byte 0
    .byte '\'
    .byte 'z'
    .byte 'x'
    .byte 'c'
    .byte 'v'
    .byte 'b'
    .byte 'n'
    .byte 'm'
    .byte ','
    .byte '.'
    .byte '/'
    .byte 0
    .byte '*'
    .skip 15
    .byte '7'
    .byte '8'
    .byte '9'
    .byte '-'
    .byte '4'
    .byte '5'
    .byte '6'
    .byte '+'
    .byte '1'
    .byte '2'
    .byte '3'
    .byte '0'
    .byte '.'
    .skip 44
    
key_shift_char_map:
    .skip 2
    .byte '!'
    .byte '@'
    .byte '#'
    .byte '$'
    .byte '%'
    .byte '^'
    .byte '&'
    .byte '*'
    .byte '('
    .byte ')'
    .byte '_'
    .byte '+'
    .skip 2
    .byte 'Q'
    .byte 'W'
    .byte 'E'
    .byte 'R'
    .byte 'T'
    .byte 'Y'
    .byte 'U'
    .byte 'I'
    .byte 'O'
    .byte 'P'
    .byte '{'
    .byte '}'
    .skip 2
    .byte 'A'
    .byte 'S'
    .byte 'D'
    .byte 'F'
    .byte 'G'
    .byte 'H'
    .byte 'J'
    .byte 'K'
    .byte 'L'
    .byte ':'
    .byte '"'
    .byte '~'
    .byte 0
    .byte '|'
    .byte 'Z'
    .byte 'X'
    .byte 'C'
    .byte 'V'
    .byte 'B'
    .byte 'N'
    .byte 'M'
    .byte '<'
    .byte '>'
    .byte '?'
    .byte 0
    .byte '*'
    .skip 18
    .byte '-'
    .skip 3
    .byte '+'
    .skip 49
    
key_extended_char_map:
    .skip 53
    .byte '/'
    .skip 74


key_code_map:
    .byte NONE
    .byte ESC_
    .skip 12 , CHAR
    .byte BACKSPACE
    .byte TAB
    .skip 12 , CHAR
    .byte ENTER_
    .byte CONTROL_LEFT
    .skip 12 , CHAR
    .byte NONE
    .skip 11 , CHAR
    .byte SHIFT_RIGHT
    .byte NUM_PAD_CHAR
    .byte ALT_LEFT
    .byte SPACE
    .byte CAPS_LOCK
    .byte F1
    .byte F2
    .byte F3
    .byte F4
    .byte F5
    .byte F6
    .byte F7
    .byte F8
    .byte F9
    .byte F10
    .byte NUM_LOCK
    .byte NONE
    .skip 13 , NUM_PAD_CHAR
    .skip 3 , NONE
    .byte F11
    .byte F12
    .skip 39 , NONE


key_shift_code_map:
    .byte NONE
    .byte ESC_
    .skip 12 , CHAR
    .byte BACKSPACE
    .byte TAB
    .skip 12 , CHAR
    .byte ENTER_
    .byte CONTROL_LEFT
    .skip 12 , CHAR
    .byte NONE
    .skip 11 , CHAR
    .byte SHIFT_RIGHT
    .byte NUM_PAD_CHAR
    .byte ALT_LEFT
    .byte SPACE
    .byte CAPS_LOCK
    .byte F1
    .byte F2
    .byte F3
    .byte F4
    .byte F5
    .byte F6
    .byte F7
    .byte F8
    .byte F9
    .byte F10
    .byte NUM_LOCK
    .byte NONE
    .byte NUM_PAD_HOME
    .byte NUM_PAD_UP
    .byte NUM_PAD_PAGE_UP
    .byte NUM_PAD_CHAR
    .byte NUM_PAD_LEFT
    .byte NONE
    .byte NUM_PAD_RIGHT
    .byte NUM_PAD_CHAR
    .byte NUM_PAD_END
    .byte NUM_PAD_DOWN
    .byte NUM_PAD_PAGE_DOWN
    .byte NUM_PAD_INS
    .byte NUM_PAD_DEL
    .skip 3 , NONE
    .byte F11
    .byte F12
    .skip 39 , NONE
    

key_extended_code_map:
    .skip 28 , NONE
    .byte NUM_PAD_ENTER
    .skip 24 , NONE
    .byte NUM_PAD_CHAR
    .skip 2 , NONE
    .byte ALT_RIGHT
    .skip 14 , NONE
    .byte HOME
    .byte UP
    .byte PAGE_UP
    .byte NONE
    .byte LEFT
    .byte NONE
    .byte RIGHT
    .byte NONE
    .byte END
    .byte DOWN
    .byte PAGE_DOWN
    .byte NONE
    .byte DEL
    .skip 44 , NONE
