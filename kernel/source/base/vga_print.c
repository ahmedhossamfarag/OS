#include "vga_print.h"
#include "vga.h"
#include "libc.h"

extern uint32_t* framebuffer;
extern uint32_t pitch;
extern uint32_t width;
extern uint32_t height;

static uint32_t vga_offset;

#define VgaLineLn (width/CHAR_WIDTH)
#define VgaNoLines (height/FONT_HEIGHT)
#define VgaEndOffset (VgaLineLn * VgaNoLines)  

void vga_scroll_up()
{
    uint32_t length = pitch * FONT_HEIGHT;
    mem_copy((char*)framebuffer + length, (char*)framebuffer, length*VgaNoLines);
    vga_offset -= VgaLineLn;
}

void vga_new_line(uint32_t bg)
{
    uint32_t current_offset = vga_offset;
    int line = vga_offset / VgaLineLn;
    line ++;
    vga_offset = line * VgaLineLn;
    for (uint32_t ofs = current_offset; ofs < vga_offset; ofs++)
    {
        vga_draw_char((ofs%VgaLineLn)*CHAR_WIDTH, (ofs/VgaLineLn)*FONT_HEIGHT, 0, bg, bg);
    }
}

void vga_print_char(char c, uint32_t fg, uint32_t bg){
    if(c == '\n'){
        vga_new_line(bg);
    }else{
        vga_draw_char((vga_offset%VgaLineLn)*CHAR_WIDTH, (vga_offset/VgaLineLn)*FONT_HEIGHT, c, fg, bg);
        vga_offset ++;
    }
    if(vga_offset >= VgaEndOffset){
        vga_scroll_up();
    }
}

void vga_print_str(const char* str, uint32_t* fg, uint32_t* bg){
    while (*str != '\0')
    {
        vga_print_char(*str, *fg, *bg);
        str++;
        fg++;
        bg++;
    }
}

void vga_print_str_fb(const char *str, uint32_t fg, uint32_t bg)
{
    while (*str != '\0')
    {
        vga_print_char(*str, fg, bg);
        str++;
    }
}

void vga_print_clear(uint32_t bg){
    vga_clear(bg);
    vga_offset = 0;
}

void vga_set_cursor(uint32_t offset, char c, uint32_t fg, uint32_t bg)
{
    if(offset > vga_offset) return;

    vga_draw_char((offset%VgaLineLn)*CHAR_WIDTH, (offset/VgaLineLn)*FONT_HEIGHT, c, fg, bg);
}

void vga_set_end_cursor(uint32_t fg, uint32_t bg)
{
    vga_set_cursor(vga_offset, 0, fg, bg);
}

void vga_set_offset(uint32_t offset)
{
    if(offset < VgaEndOffset){
        vga_offset = offset;
    }
}

uint32_t vga_get_offset()
{
    return vga_offset;
}
