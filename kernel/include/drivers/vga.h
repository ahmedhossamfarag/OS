#define VGA_PALETTE_INDEX 0x3C8
#define VGA_PALETTE_DATA 0x3C9

#include <stdint.h>

void vga_init();

uint8_t vga_available();

void vga_set_palette_entry(uint8_t index, uint8_t red, uint8_t green, uint8_t blue);

void vga_clear(uint8_t color);