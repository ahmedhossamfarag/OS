#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include "winlib.h"   /* window_t */

/* Pixels */
void     set_pixel(window_t* window, int x, int y, uint32_t color);
uint32_t get_pixel(window_t* window, int x, int y);
void     set_pixel_alpha(window_t* window, int x, int y, uint32_t color); /* 0xAARRGGBB */
void     clear_window(window_t* window, uint32_t color);

/* Lines */
void draw_hline(window_t* window, int x0, int x1, int y, uint32_t color);
void draw_vline(window_t* window, int x, int y0, int y1, uint32_t color);
void draw_line(window_t* window, int x0, int y0, int x1, int y1, uint32_t color);
void draw_thick_line(window_t* window, int x0, int y0, int x1, int y1, int thickness, uint32_t color);

/* Rectangles */
void draw_rectangle(window_t* window, int x, int y, int width, int height, uint32_t color);
void fill_rect(window_t* window, int x, int y, int width, int height, uint32_t color);
void fill_rect_alpha(window_t* window, int x, int y, int width, int height, uint32_t color);
void draw_rectangle_thick(window_t* window, int x, int y, int width, int height, int thickness, uint32_t color);
void draw_rounded_rect(window_t* window, int x, int y, int width, int height, int radius, uint32_t color);
void fill_rounded_rect(window_t* window, int x, int y, int width, int height, int radius, uint32_t color);

/* Circles & ellipses */
void draw_circle(window_t* window, int xc, int yc, int radius, uint32_t color);
void fill_circle(window_t* window, int xc, int yc, int radius, uint32_t color);
void draw_ring(window_t* window, int xc, int yc, int outer_r, int inner_r, uint32_t color);
void draw_ellipse(window_t* window, int xc, int yc, int rx, int ry, uint32_t color);
void fill_ellipse(window_t* window, int xc, int yc, int rx, int ry, uint32_t color);

/* Triangles & polygons (pts = {x0,y0,x1,y1,...}) */
void draw_triangle(window_t* window, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void fill_triangle(window_t* window, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_polygon(window_t* window, const int* pts, int count, uint32_t color);
void fill_polygon(window_t* window, const int* pts, int count, uint32_t color);

/* Gradients */
void fill_gradient_v(window_t* window, int x, int y, int width, int height, uint32_t top, uint32_t bottom);
void fill_gradient_h(window_t* window, int x, int y, int width, int height, uint32_t left, uint32_t right);

/* Bitmaps */
void draw_bitmap(window_t* window, int x, int y, const uint32_t* bitmap, int w, int h);
void draw_bitmap_alpha(window_t* window, int x, int y, const uint32_t* bitmap, int w, int h);
void blit(window_t* dst, int dx, int dy, window_t* src, int sx, int sy, int w, int h);

/* Flood fill: stack needs 2 * stack_capacity ints; returns 0 ok, -1 on overflow */
int flood_fill(window_t* window, int x, int y, uint32_t new_color, int* stack, int stack_capacity);

#endif