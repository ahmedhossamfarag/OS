#include "stdint.h"

typedef struct
{
    int32_t locationx;
    int32_t locationy;
    int32_t width;
    int32_t height;
} bounds_t;

typedef struct {

    bounds_t bounds;
    uint32_t* owner;
    uint32_t* buffer;
} window_t;

window_t* create_window(int32_t x, int32_t y, uint32_t width, uint32_t height, uint32_t bgcolor);

void* register_window(window_t* window);

uint8_t deregister_window(void* window_handle);

uint8_t redraw_window(void* window_handle);

uint8_t update_window_bounds(void* window_handle, bounds_t* bounds);