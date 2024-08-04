#include "graphics.h"
#include "vga.h"
#include "math.h"

extern uint16_t pitch;
extern uint16_t width;
extern uint16_t height;
extern uint8_t* font_map;

uint8_t* back_buffer;

void graphics_init()
{
    back_buffer = (uint8_t*) BACk_BUFFER_OFFSET;
    graphics_clear(0);
}

void set_pixel(int x, int y, uint8_t color) {
    if(x < width && y < height){
        back_buffer[y * pitch + x] = color;
    }
}

/* Bresenham's Line Algorithm */
void draw_line(int x0, int y0, int x1, int y1, uint8_t color) {
    int dx = math_abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -math_abs(y1 - y0), sy = y0 < y1 ? 1 : -1; 
    int err = dx + dy, e2;

    while (1) {
        set_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}


void draw_rectangle(int x, int y, int width, int height, uint8_t color) {
    for (int i = 0; i < width; i++) {
        set_pixel(x + i, y, color);
        set_pixel(x + i, y + height - 1, color);
    }
    for (int i = 0; i < height; i++) {
        set_pixel(x, y + i, color);
        set_pixel(x + width - 1, y + i, color);
    }
}

/* Midpoint Circle Algorithm */
void draw_circle(int xc, int yc, int radius, uint8_t color) {
    int x = radius, y = 0;
    int p = 1 - radius;

    while (x >= y) {
        set_pixel(xc + x, yc + y, color);
        set_pixel(xc + y, yc + x, color);
        set_pixel(xc - y, yc + x, color);
        set_pixel(xc - x, yc + y, color);
        set_pixel(xc - x, yc - y, color);
        set_pixel(xc - y, yc - x, color);
        set_pixel(xc + y, yc - x, color);
        set_pixel(xc + x, yc - y, color);
        y++;
        if (p <= 0) {
            p = p + 2*y + 1;
        } else {
            x--;
            p = p + 2*y - 2*x + 1;
        }
    }
}

void graphics_clear(uint8_t color){
    uint32_t len = width * height;
    for (uint8_t* pixel = back_buffer; pixel < back_buffer + len; pixel++)
    {
        *pixel = color;
    }
}


void graphics_update(){
    vga_copy_buffer(back_buffer);
}