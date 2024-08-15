#define VGA_PALETTE_INDEX 0x3C8
#define VGA_PALETTE_DATA 0x3C9

#define FONT_HEIGHT 16 

#include <stdint.h>

void vga_init();

uint8_t vga_available();

void put_pixel_rgb(uint32_t x, uint32_t y, uint32_t color);

void vga_clear(uint32_t color);

void vga_draw_char(int x, int y, char c, uint32_t fg, uint32_t bg);

void vga_copy_buffer(uint32_t* buffer);