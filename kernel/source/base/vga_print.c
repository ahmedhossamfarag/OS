#include "vga_print.h"
#include "vga.h"
#include "libc.h"

extern uint8_t* framebuffer;
extern uint16_t pitch;
extern uint16_t width;
extern uint16_t height;

uint32_t vga_offset;

#define VgaLineLn (width/CHAR_WIDTH)
#define VgaNoLines (height/FONT_HEIGHT)
#define VgaEndOffset (VgaLineLn * VgaNoLines)  

void vga_scroll_up()
{
    uint32_t line_ln = pitch * FONT_HEIGHT;
    mem_copy(framebuffer + line_ln, framebuffer, line_ln*VgaNoLines);
    vga_offset -= VgaLineLn;
}

void vga_new_line()
{
    uint32_t current_offset = vga_offset;
    int line = vga_offset / VgaLineLn;
    line ++;
    vga_offset = line * VgaLineLn;
    for (uint32_t ofs = current_offset; ofs < vga_offset; ofs++)
    {
        vga_draw_char((ofs%VgaLineLn)*CHAR_WIDTH, (ofs/VgaLineLn)*FONT_HEIGHT, 0, 0);
    }
    
    
}

void vga_print(char c){
    if(vga_offset >= VgaEndOffset){
        vga_scroll_up();
    }
    if(c == '\n'){
        vga_new_line();
    }else{
        vga_draw_char((vga_offset%VgaLineLn)*CHAR_WIDTH, (vga_offset/VgaLineLn)*FONT_HEIGHT, c, VGA_PRINT_COLOR);
        vga_offset ++;
    }
}

void vga_print_char(char c){
    vga_print(c);
    vga_set_cursor(vga_offset);
}

void vga_print_str(const char* str){
    while (*str != '\0')
    {
        vga_print(*str);
        str++;
    }
    vga_set_cursor(vga_offset);  
}

void vga_print_clear(){
    
    vga_clear(0);
    vga_offset = 0;
    vga_set_cursor(vga_offset);
}

void vga_set_cursor(int offset)
{
    vga_draw_char((offset%VgaLineLn)*CHAR_WIDTH, (offset/VgaLineLn)*FONT_HEIGHT, '_', VGA_PRINT_COLOR);
}
