#define BACk_BUFFER_OFFSET 0xE000000

#include <stdint.h>

void graphics_init();

void set_pixel(int x, int y, uint32_t color);

/* Bresenham's Line Algorithm */
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);

void draw_rectangle(int x, int y, int width, int height, uint32_t color);

/* Midpoint Circle Algorithm */
void draw_circle(int xc, int yc, int radius, uint32_t color);

void graphics_clear(uint32_t color);

void graphics_update();