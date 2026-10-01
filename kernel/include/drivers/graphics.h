#include <stdint.h>

void graphics_init();

void graphics_clear(uint32_t color);

void graphics_update();

void graphics_write(uint32_t* buffer, int32_t x, int32_t y, uint32_t w, uint32_t h);

void graphics_read(uint32_t* buffer, int32_t x, int32_t y, uint32_t w, uint32_t h);
