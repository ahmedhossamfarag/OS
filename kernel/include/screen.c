#include "low_level.h"
#include "screen.h"

void screen_print(int offset, char c){
    char* pntr = (char*) VideoMemoryStart + (offset * 2);
    *pntr = c;
}

void screen_print_str(int offset, char* str){
    char* pntr = (char*) VideoMemoryStart + (offset * 2);
    while (*str != '\0')
    {
        *pntr = *str;
        pntr += 2;
        str++;
    }  
}

void screen_clear(int offset){
    if(offset < 0){
        offset = VideoMemoryEndOffset;
    }
    
    char* pntr = (char*) VideoMemoryStart;
    for (int i = 0; i < offset; i++)
    {
        *pntr = 0;
        pntr += 2;
    }
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
