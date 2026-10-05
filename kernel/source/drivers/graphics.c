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

uint32_t arrow_cursor_map[CURSOR_LENGTH] = {
    0x40000000, // B
    0x50000000, // BB
    0x64000000, // BWB
    0x69000000, // BWWB
    0x6A400000, // BWWWB
    0x6A900000, // BWWWWB
    0x6AA40000, // BWWWWWWB
    0x6AA90000, // BWWWWWWWB
    0x6AAA4000, // BWWWWWWWWB
    0x6AAA9000, // BWWWWWWWWWB
    0x6A955500, // BWWWWBBBBBBB 
    0x6A400000, // BWWWB
    0x69000000, // BWWB
    0x69000000, // BWWB
    0x64000000, // BWB
    0x50000000  // BB
};

static struct
{
    int16_t x;
    int16_t y;
    int8_t visible;
    uint32_t* cursor_back_buffer;
} cursor_args;


void graphics_init()
{
    back_buffer = (uint32_t*) alloc(pitch*height);
    graphics_clear(0);

    cursor_args.cursor_back_buffer = (uint32_t*) alloc(CURSOR_LENGTH*CURSOR_LENGTH*sizeof(uint32_t));
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


void graphics_update_region(int32_t x, int32_t y, uint32_t w, uint32_t h){
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

    for (uint32_t row = 0; row < ch; row++)
    {
        uint32_t offset = ((y + row) * pixels_per_scanline) + x;
        uint32_t* src = back_buffer + offset;
        vga_copy_sz_buffer(src, offset, cw);

    }
}


void graphics_write(uint32_t* buffer, int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t pbsl) { // pbsl = pixels per scanline
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
    buffer += offsety * pbsl;

    uint32_t cpitch = cw * sizeof(uint32_t);

    for (uint32_t i = 0; i < ch; i++)
    {
        mem_copy((char*)buffer, (char*)write_pntr, cpitch);
        write_pntr += pixels_per_scanline;
        buffer += pbsl;
    }
}

void graphics_read(uint32_t* buffer, int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t pbsl) { // pbsl = pixels per scanline
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
    buffer += offsety * pbsl;

    uint32_t cpitch = cw * sizeof(uint32_t);

    for (uint32_t i = 0; i < ch; i++)
    {
        mem_copy((char*)read_pntr, (char*)buffer, cpitch);
        read_pntr += pixels_per_scanline;
        buffer += pbsl;
    }
}

void cursor_write_back(){
    int x = cursor_args.x,
        y = cursor_args.y,
        cw = CURSOR_LENGTH,
        ch = CURSOR_LENGTH,
        pbsl = CURSOR_LENGTH;
    uint32_t* buffer = cursor_args.cursor_back_buffer;
    uint32_t* write_pntr = back_buffer + (y * pixels_per_scanline) + x;

    uint32_t cpitch = cw * sizeof(uint32_t);

    for (int i = 0; i < ch; i++)
    {
        mem_copy((char*)buffer, (char*)write_pntr, cpitch);
        write_pntr += pixels_per_scanline;
        buffer += pbsl;
    }
}

void cursor_draw(){
    int cursor_x = cursor_args.x,
        cursor_y = cursor_args.y;

    for(int y = 0; y < CURSOR_LENGTH; y++){
        for(int x = 0; x < CURSOR_LENGTH; x++){
            uint8_t sh = CURSOR_LENGTH - x - 1;
            uint8_t pixel = ((arrow_cursor_map[y] >> sh )>> sh) & 0b11;
            uint32_t* pntr = back_buffer 
                                + ((cursor_y + y) * pixels_per_scanline) 
                                + cursor_x + x;

            switch (pixel)
            {
            case 0b01:
                *pntr = 0x0;
                break;
            case 0b10:
                *pntr = 0xFFFFFFFF;
            default:
                break;
            }
        }
    }
}

void cursor_read_back(){
    int x = cursor_args.x,
        y = cursor_args.y,
        cw = CURSOR_LENGTH,
        ch = CURSOR_LENGTH,
        pbsl = CURSOR_LENGTH;
    uint32_t* buffer = cursor_args.cursor_back_buffer;
    uint32_t* read_pntr = back_buffer + (y * pixels_per_scanline) + x;

    uint32_t cpitch = cw * sizeof(uint32_t);

    for (int i = 0; i < ch; i++)
    {
        mem_copy((char*)read_pntr, (char*)buffer, cpitch);
        read_pntr += pixels_per_scanline;
        buffer += pbsl;
    }

    cursor_draw();
}

void graphics_cursor(uint32_t cursor_x, uint32_t cursor_y){
    if(cursor_args.visible){
        cursor_write_back();
        graphics_update_region(cursor_args.x, cursor_args.y, CURSOR_LENGTH, CURSOR_LENGTH);
    }


    if (cursor_x > width - CURSOR_LENGTH)
        cursor_x = width - CURSOR_LENGTH;
    if (cursor_y > height -CURSOR_LENGTH)
        cursor_y = height - CURSOR_LENGTH;

    
    cursor_args.x = cursor_x;
    cursor_args.y = cursor_y;
    cursor_args.visible = 1;

    cursor_read_back();
    
    graphics_update_region(cursor_x, cursor_y, CURSOR_LENGTH, CURSOR_LENGTH);
}