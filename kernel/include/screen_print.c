#include "screen_print.h"
#include "low_level.h"

char* screen_0 = (char*) VideoMemoryStart;
char* screen_pntr = (char*) VideoMemoryStart;
int screen_offset = 0;

void screen_new_line()
{
    int line = screen_offset / VideoMemoryLineLn;
    line ++;
    screen_offset = line * VideoMemoryLineLn;
    screen_pntr = screen_0 + (screen_offset * 2);
}

void screen_print(char c){
    if(c == '\n'){
        screen_new_line();
    }else{
        *screen_pntr = c;
        screen_pntr++;
        *screen_pntr = 0x0f;
        screen_pntr++;
        screen_offset ++;
    }
}

void screen_print_char(char c){
    screen_print(c);
    screen_set_cursor(screen_offset);
}

void screen_print_str(const char* str){
    while (*str != '\0')
    {
        screen_print(*str);
        str++;
    }
    screen_set_cursor(screen_offset);  
}

void screen_clear(){
    
    char* end_pntr = screen_0 + (VideoMemoryEndOffset * 2);
    for (char* pntr = screen_0; pntr < end_pntr; pntr += 2)
    {
        *pntr = 0;
    }
    screen_pntr = screen_0;
    screen_offset = 0;
}

void screen_set_cursor(int offset)
{
    port_byte_out ( REG_SCREEN_CTRL , 14); // Set offset high byte
    port_byte_out ( REG_SCREEN_DATA , ( unsigned char )( offset >> 8));
    port_byte_out ( REG_SCREEN_CTRL , 15); // Set offset low byte
    port_byte_out ( REG_SCREEN_DATA , ( unsigned char )( offset));
}

int screen_get_cursor()
{
    int offset;
    port_byte_out ( REG_SCREEN_CTRL , 14); // Get offset high byte
    offset = port_byte_in ( REG_SCREEN_DATA) << 8;
    port_byte_out ( REG_SCREEN_CTRL , 15); // Get offset low byte
    offset += port_byte_in ( REG_SCREEN_DATA);
    return offset;
}