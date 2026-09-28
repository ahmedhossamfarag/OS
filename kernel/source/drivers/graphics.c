#include "graphics.h"
#include "vga.h"
#include "math.h"
#include "memory.h"
#include "libc.h"

extern uint32_t pitch;
extern uint32_t width;
extern uint32_t height;
extern uint32_t pixels_per_scanline;
extern uint8_t* font_map;

uint32_t* back_buffer;

void graphics_init()
{
    back_buffer = (uint32_t*) alloc(pitch*height);
    graphics_clear(0);
}

void graphics_clear(uint32_t color){
    uint32_t len = pixels_per_scanline * height;
    for (uint32_t* pixel = back_buffer; pixel < back_buffer + len; pixel++)
    {
        *pixel = color;
    }
}


void graphics_update(){
    vga_copy_buffer(back_buffer);
}

void graphics_write(uint32_t* buffer, int32_t x, int32_t y, uint32_t w, uint32_t h) {
    if (x >= (int32_t)width || y >= (int32_t)height || x + w <= 0 || y + h <= 0) return;

    uint32_t offsetx= 0, offsety = 0;
    if (x < 0) {
        offsetx = -x;
        x = 0;
    }
    if (y < 0) {
        offsety = -y;
        y = 0;
    }

    uint32_t cw = w - offsetx;
    uint32_t ch = h - offsety;
    cw = math_min(cw, width - x);
    ch = math_min(ch, height - y);

    uint32_t* write_pntr = back_buffer + (y * pixels_per_scanline) + x;
    buffer += offsety * w;

    uint32_t cpitch = cw * sizeof(uint32_t);

    for (uint32_t i = 0; i < ch; i++)
    {
        mem_copy((char*)buffer, (char*)write_pntr, cpitch);
        write_pntr += pixels_per_scanline;
        buffer += w;
    }
}

void graphics_read(uint32_t* buffer, int32_t x, int32_t y, uint32_t w, uint32_t h) {
    if (x >= (int32_t)width || y >= (int32_t)height || x + w <= 0 || y + h <= 0) return;

    uint32_t offsetx= 0, offsety = 0;
    if (x < 0) {
        offsetx = -x;
        x = 0;
    }
    if (y < 0) {
        offsety = -y;
        y = 0;
    }

    uint32_t cw = w - offsetx;
    uint32_t ch = h - offsety;
    cw = math_min(cw, width - x);
    ch = math_min(ch, height - y);

    uint32_t* read_pntr = back_buffer + (y * pixels_per_scanline) + x;
    buffer += offsety * w;

    uint32_t cpitch = cw * sizeof(uint32_t);

    for (uint32_t i = 0; i < ch; i++)
    {
        mem_copy((char*)read_pntr, (char*)buffer, cpitch);
        read_pntr += pixels_per_scanline;
        buffer += w;
    }
}
