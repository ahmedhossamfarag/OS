#include "print.h"

int screen_offset = 0;

void print_new_line()
{
    int line = screen_offset / VideoMemoryLineLn;
    line ++;
    screen_offset = line * VideoMemoryLineLn;
}

void print_no_cursor(char c)
{
    if(c == '\n'){
        print_new_line();
    }else{
        screen_print(screen_offset, c);
        screen_offset ++;
    }
}

void print(char c)
{
    print_no_cursor(c);
    screen_set_cursor(screen_offset);
}

void print_str_no_cursor(char *str)
{
    while(*str != '\0'){
        print_no_cursor(*str);
        str ++;
    }
}

void print_str(char *str)
{
    print_str_no_cursor(str);
    screen_set_cursor(screen_offset);
}
