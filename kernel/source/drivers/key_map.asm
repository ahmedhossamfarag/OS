global key_char_map
global key_shift_char_map
global key_extended_char_map
global key_code_map
global key_shift_code_map
global key_extended_code_map

NONE equ 0
ESC_ equ 1
CHAR equ 2
BACKSPACE equ 3
TAB equ 4
ENTER_ equ 5
CONTROL_LEFT equ 6
SHIFT_RIGHT equ 7
NUM_PAD_CHAR equ 8
ALT_LEFT equ 9
SPACE equ 10
CAPS_LOCK equ 11
F1 equ 12
F2 equ 13
F3 equ 14
F4 equ 15
F5 equ 16
F6 equ 17
F7 equ 18
F8 equ 19
F9 equ 20
F10 equ 21
F11 equ 22
F12 equ 23
NUM_LOCK equ 24
NUM_PAD_HOME equ 25
NUM_PAD_UP equ 26
NUM_PAD_PAGE_UP equ 27
NUM_PAD_LEFT equ 28
NUM_PAD_RIGHT equ 29
NUM_PAD_END equ 30
NUM_PAD_DOWN equ 31
NUM_PAD_PAGE_DOWN equ 32
NUM_PAD_INS equ 33
NUM_PAD_DEL equ 34
NUM_PAD_ENTER equ 35
ALT_RIGHT equ 36
HOME equ 37
UP equ 38
PAGE_UP equ 39
LEFT equ 40
RIGHT equ 41
END equ 42
DOWN equ 43
PAGE_DOWN equ 44
DEL equ 45


key_char_map:
    times 2 db 0
    db '1'
    db '2'
    db '3'
    db '4'
    db '5'
    db '6'
    db '7'
    db '8'
    db '9'
    db '0'
    db '-'
    db '='
    times 2 db 0
    db 'q'
    db 'w'
    db 'e'
    db 'r'
    db 't'
    db 'y'
    db 'u'
    db 'i'
    db 'o'
    db 'p'
    db '['
    db ']'
    times 2 db 0
    db 'a'
    db 's'
    db 'd'
    db 'f'
    db 'g'
    db 'h'
    db 'j'
    db 'k'
    db 'l'
    db ';'
    db '''
    db '`'
    db 0
    db '\'
    db 'z'
    db 'x'
    db 'c'
    db 'v'
    db 'b'
    db 'n'
    db 'm'
    db ','
    db '.'
    db '/'
    db 0
    db '*'
    times 15 db 0
    db '7'
    db '8'
    db '9'
    db '-'
    db '4'
    db '5'
    db '6'
    db '+'
    db '1'
    db '2'
    db '3'
    db '0'
    db '.'
    times 44 db 0
    
key_shift_char_map:
    times 2 db 0
    db '!'
    db '@'
    db '#'
    db '$'
    db '%'
    db '^'
    db '&'
    db '*'
    db '('
    db ')'
    db '_'
    db '+'
    times 2 db 0
    db 'Q'
    db 'W'
    db 'E'
    db 'R'
    db 'T'
    db 'Y'
    db 'U'
    db 'I'
    db 'O'
    db 'P'
    db '{'
    db '}'
    times 2 db 0
    db 'A'
    db 'S'
    db 'D'
    db 'F'
    db 'G'
    db 'H'
    db 'J'
    db 'K'
    db 'L'
    db ':'
    db '"'
    db '~'
    db 0
    db '|'
    db 'Z'
    db 'X'
    db 'C'
    db 'V'
    db 'B'
    db 'N'
    db 'M'
    db '<'
    db '>'
    db '?'
    db 0
    db '*'
    times 18 db 0
    db '-'
    times 3 db 0
    db '+'
    times 49 db 0
    
key_extended_char_map:
    times 53 db 0
    db '/'
    times 74 db 0


key_code_map:
    db NONE
    db ESC_
    times 12 db CHAR
    db BACKSPACE
    db TAB
    times 12 db CHAR
    db ENTER_
    db CONTROL_LEFT
    times 12 db CHAR
    db NONE
    times 11 db CHAR
    db SHIFT_RIGHT
    db NUM_PAD_CHAR
    db ALT_LEFT
    db SPACE
    db CAPS_LOCK
    db F1
    db F2
    db F3
    db F4
    db F5
    db F6
    db F7
    db F8
    db F9
    db F10
    db NUM_LOCK
    db NONE
    times 13 db NUM_PAD_CHAR
    times 3 db NONE
    db F11
    db F12
    times 39 db NONE


key_shift_code_map:
    db NONE
    db ESC_
    times 12 db CHAR
    db BACKSPACE
    db TAB
    times 12 db CHAR
    db ENTER_
    db CONTROL_LEFT
    times 12 db CHAR
    db NONE
    times 11 db CHAR
    db SHIFT_RIGHT
    db NUM_PAD_CHAR
    db ALT_LEFT
    db SPACE
    db CAPS_LOCK
    db F1
    db F2
    db F3
    db F4
    db F5
    db F6
    db F7
    db F8
    db F9
    db F10
    db NUM_LOCK
    db NONE
    db NUM_PAD_HOME
    db NUM_PAD_UP
    db NUM_PAD_PAGE_UP
    db NUM_PAD_CHAR
    db NUM_PAD_LEFT
    db NONE
    db NUM_PAD_RIGHT
    db NUM_PAD_CHAR
    db NUM_PAD_END
    db NUM_PAD_DOWN
    db NUM_PAD_PAGE_DOWN
    db NUM_PAD_INS
    db NUM_PAD_DEL
    times 3 db NONE
    db F11
    db F12
    times 39 db NONE
    

key_extended_code_map:
    times 28 db NONE
    db NUM_PAD_ENTER
    times 24 db NONE
    db NUM_PAD_CHAR
    times 2 db NONE
    db ALT_RIGHT
    times 14 db NONE
    db HOME
    db UP
    db PAGE_UP
    db NONE
    db LEFT
    db NONE
    db RIGHT
    db NONE
    db END
    db DOWN
    db PAGE_DOWN
    db NONE
    db DEL
    times 44 db NONE
    