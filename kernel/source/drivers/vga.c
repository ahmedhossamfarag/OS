#include "vga.h"
#include "low_level.h"
#include "info.h"
#include "memory.h"
#include "libc.h"

uint32_t* framebuffer;
uint32_t pitch;
uint32_t width;
uint32_t height;
uint8_t* font_map;

void vga_init()
{
    graphics_info_t* g = info_get_graphics();
    framebuffer = (uint32_t*)g->FameBufferBase;
    pitch = g->PixelsPerScanLine * 4;
    width = g->Width;
    height = g->Height;
    extern uint8_t _font_bitmap[];
    font_map = _font_bitmap;
}

uint8_t vga_available()
{
    return info_get_graphics()->PixelFormat != 0xFFFF;
}

void put_pixel_rgb(uint32_t x, uint32_t y, uint32_t color){
    framebuffer[x + y * width] = color;
}

void vga_clear(uint32_t color)
{
    uint32_t len = width * height;
    for (uint32_t* pixel = framebuffer; pixel < framebuffer + len; pixel++)
    {
        *pixel = color;
    }
}

void vga_draw_char(int x, int y, char c, uint32_t fg, uint32_t bg) {
    uint8_t *glyph = font_map + c * FONT_HEIGHT;
    for (int i = 0; i < FONT_HEIGHT; i++) {
        uint8_t line = glyph[i];
        for (int j = 0; j < 8; j++) {
            if (line & (0x80 >> j)) {
                put_pixel_rgb(x + j, y + i, fg);
            }else{
                put_pixel_rgb(x + j, y + i, bg);
            }
        }
    }
}

void vga_copy_buffer(uint32_t *buffer)
{
    mem_copy((char*)buffer, (char*)framebuffer, pitch*height);
}
