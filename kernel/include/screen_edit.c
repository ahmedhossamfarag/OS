#include "screen_edit.h"
#include "low_level.h"

#define current_line screen_edit_offset / VideoMemoryLineLn

char* screen_edit_0;
char* screen_edit_end;
char* screen_edit_pntr;
int screen_edit_offset;


void screen_edit_init() {
    screen_edit_0 = (char*)VideoMemoryStart;
    screen_edit_end = (char*)VideoMemoryStart + (VideoMemoryEndOffset * 2);
    screen_edit_pntr = (char*)VideoMemoryStart;
    screen_edit_offset = 0;
}

void screen_edit_set_cursor(int offset)
{
    port_byte_out(REG_SCREEN_CTRL, 14); // Set offset high byte
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15); // Set offset low byte
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset));
}

void screen_edit_shift_right(char* from, char* to) {
    char* start = from;
    if(*from == '\0') return;
    while (to < screen_edit_end - 2 && *(from+2) != '\0') {
        from += 2;
        to += 2;
    }
    while (from >= start) {
        *to = *from;
        to -= 2;
        from -= 2;
    }
    while (to >= start)
    {
        *to = ' ';
        to -= 2;
    }
}

void screen_edit_shift_left(char* from, char* to) {
    if (from < screen_edit_end) {
        do {
            *to = *from;
            to += 2;
            from += 2;
        } while (from < screen_edit_end && *from != '\0');
        *to = '\0';
    }
    else
    {
        *to = '\0';
    }
}

void screen_edit_print(char c) {
    screen_edit_shift_right(screen_edit_pntr, screen_edit_pntr + 2);
    *screen_edit_pntr = c;
    screen_edit_pntr++;
    *screen_edit_pntr = 0x0f;
    screen_edit_pntr++;
    screen_edit_offset++;
}

void screen_edit_print_char(char c) {
    if (screen_edit_offset < VideoMemoryEndOffset - 1) {
        screen_edit_print(c);
        screen_edit_set_cursor(screen_edit_offset);
    }
}

void screen_edit_print_str(const char* str) {
    while (screen_edit_offset < VideoMemoryEndOffset - 1 && *str != '\0')
    {
        screen_edit_print(*str);
        str++;
    }
    screen_edit_set_cursor(screen_edit_offset);
}

void screen_edit_del_before()
{
    if (screen_edit_offset > 0) {
        screen_edit_shift_left(screen_edit_pntr, screen_edit_pntr - 2);
        screen_edit_pntr -= 2;
        screen_edit_offset--;
        screen_edit_set_cursor(screen_edit_offset);
    }
}

void screen_edit_del_after()
{
    screen_edit_shift_left(screen_edit_pntr + 2, screen_edit_pntr);
}

void screen_edit_cursor_up()
{
    int line = current_line;
    if (line > 0) {
        screen_edit_offset -= VideoMemoryLineLn;
        screen_edit_pntr -= VideoMemoryLineLn * 2;
        screen_edit_set_cursor(screen_edit_offset);
    }
}

void screen_edit_cursor_down()
{
    int line = current_line;
    if (line < VideoMemoryEndLine - 1) {
        screen_edit_offset += VideoMemoryLineLn;
        screen_edit_pntr += VideoMemoryLineLn * 2;
        if (*(screen_edit_pntr - 2) == '\0') {
            screen_edit_cursor_left();
        }else{
            screen_edit_set_cursor(screen_edit_offset);
        }
    }
}

void screen_edit_cursor_left()
{
    if (screen_edit_offset > 0) {
        do {
            screen_edit_pntr -= 2;
            screen_edit_offset--;
        } while (screen_edit_offset > 0 && *(screen_edit_pntr - 2) == '\0');
        screen_edit_set_cursor(screen_edit_offset);
    }
}

void screen_edit_cursor_right()
{
    if (screen_edit_offset < VideoMemoryEndOffset - 1)
    {
        if (*screen_edit_pntr != '\0') {
            screen_edit_pntr += 2;
            screen_edit_offset++;
            screen_edit_set_cursor(screen_edit_offset);
        }
    }
}

void screen_edit_new_line()
{
    int line = current_line;
    if (line < VideoMemoryEndLine - 1) {
        char* current_pntr = screen_edit_pntr;
        line++;
        screen_edit_offset = line * VideoMemoryLineLn;
        screen_edit_pntr = screen_edit_0 + (screen_edit_offset * 2);
        screen_edit_shift_right(current_pntr, screen_edit_pntr);
        screen_edit_set_cursor(screen_edit_offset);
    }
}

void screen_edit_clear() {
    for (char* pntr = screen_edit_0; pntr < screen_edit_end; pntr += 2)
    {
        *pntr = 0;
    }
    screen_edit_pntr = screen_edit_0;
    screen_edit_offset = 0;
    screen_edit_set_cursor(screen_edit_offset);
}

