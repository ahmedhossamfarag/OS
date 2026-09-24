#include "windows.h"
#include "memory.h"
#include "dslib.h"
#include "graphics.h"
#include "scheduler.h"

window_t* windows;

queue_t* windows_queue;

void windows_init(){
    windows = (window_t*) alloc(MAX_WINDOWS * sizeof(window_t));
    windows_queue = queue_new(MAX_WINDOWS, alloc);
}

void draw_window(window_t* window){
    graphics_write(window->buffer, window->bounds.locationx, window->bounds.locationy, window->bounds.width, window->bounds.height);
    graphics_update();
}

window_t* register_window(window_t* window){
    window_t* window_pntr = 0;

    for (int i = 0; i < MAX_WINDOWS; i++)
    {
        if(windows[i].owner == 0){
            window_pntr = &windows[i];
            break;
        }
    }

    if (window_pntr == 0)
    {
        return 0;
    }

    *window_pntr = *window;
    window_pntr->owner = get_current_thread();

    queue_inque(windows_queue, window_pntr);

    draw_window(window_pntr);
    
    return window_pntr;
}

uint8_t owns_window(window_t* window) {
    thread_t* current_thread = get_current_thread();

    for (window_t* w = windows; w < windows + MAX_WINDOWS; w++)
    {
        if (w == window)
        {
            if (w->owner == current_thread)
            {
                return 1;
            }
            return 0;
        }
    }
    return 0;
}

uint8_t unregister_window(window_t* window){
    if (owns_window(window)){
        window->owner = 0;
        queue_remove(windows_queue, window);
        windows_redraw();
    }
    return 0;
}

uint8_t window_focus(window_t* window){
    if (owns_window(window)) {
        queue_remove(windows_queue, window);
        queue_inque(windows_queue, window);
        draw_window(window);
    }
    return 0;
}

uint8_t update_window_bounds(window_t* window, bounds_t* bounds){
    if (owns_window(window)){
        window->bounds = *bounds;
        windows_redraw();
    }
    return 0;
}

void redraw_window(window_t* window){
    if (owns_window(window)){
        graphics_write(window->buffer, window->bounds.locationx, window->bounds.locationy, window->bounds.width, window->bounds.height);
        graphics_update();
    }
}

void windows_redraw(){
    graphics_clear(0);

    for (uint32_t i = 0; i < windows_queue->size; i++)
    {
        window_t* window = windows_queue->data[(windows_queue->head + i) % windows_queue->capacity];
        graphics_write(window->buffer, window->bounds.locationx, window->bounds.locationy, window->bounds.width, window->bounds.height);
    }

    graphics_update();
}