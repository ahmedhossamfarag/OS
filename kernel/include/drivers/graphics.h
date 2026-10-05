#include <stdint.h>

#define CURSOR_LENGTH 16

void graphics_init();

void graphics_clear(uint32_t color);

void graphics_update();

void graphics_update_region(int32_t x, int32_t y, uint32_t w, uint32_t h);

void graphics_write(uint32_t* buffer, int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t pbsl); // pbsl = pixels per scanline

void graphics_read(uint32_t* buffer, int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t pbsl); // pbsl = pixels per scanline

void graphics_cursor(uint32_t cursor_x, uint32_t cursor_y);
