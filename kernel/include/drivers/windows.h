#include "stdint.h"
#include "process.h"

#define MAX_WINDOWS 10

typedef struct
{
    int32_t locationx;
    int32_t locationy;
    int32_t width;
    int32_t height;
} __attribute__((packed)) bounds_t;

typedef struct {

    bounds_t bounds;
    thread_t* owner;
    uint32_t* buffer;
} __attribute__((packed)) window_t;

void windows_init();

window_t* register_window(window_t* window);

uint8_t unregister_window(window_t* window);

uint8_t window_focus(window_t* window);

uint8_t update_window_bounds(window_t* window, bounds_t* bounds);

void redraw_window(window_t* window);

void redraw_window_region(window_t* window, bounds_t* bounds);

window_t* get_active_window();

window_t* get_window_at(int32_t x, int32_t y);