#include "windows.h"
#include "memory.h"
#include "dslib.h"
#include "graphics.h"
#include "scheduler.h"
#include "math.h"

window_t* windows;

queue_t* windows_queue;

window_t* active_window;


void windows_init(){
    windows = (window_t*) alloc(MAX_WINDOWS * sizeof(window_t));
    windows_queue = queue_new(MAX_WINDOWS, alloc);
    active_window = 0;
}


static bounds_t absolute_bounds(window_t* window, bounds_t* rel_bounds){
    bounds_t abs_bounds;
    abs_bounds.locationx = window->bounds.locationx + rel_bounds->locationx;
    abs_bounds.locationy = window->bounds.locationy + rel_bounds->locationy;
    abs_bounds.width = rel_bounds->width;
    abs_bounds.height = rel_bounds->height;

    return abs_bounds;
}

static bounds_t relative_bounds(window_t* window, bounds_t* abs_bounds){
    bounds_t rel_bounds;
    rel_bounds.locationx = abs_bounds->locationx - window->bounds.locationx;
    rel_bounds.locationy = abs_bounds->locationy - window->bounds.locationy;
    rel_bounds.width = abs_bounds->width;
    rel_bounds.height = abs_bounds->height;

    return rel_bounds;
}

static bounds_t interset_bounds(bounds_t* a, bounds_t* b){
    bounds_t intersection;

    int32_t x1 = math_max(a->locationx, b->locationx);
    int32_t y1 = math_max(a->locationy, b->locationy);
    int32_t x2 = math_min(a->locationx + a->width, b->locationx + b->width);
    int32_t y2 = math_min(a->locationy + a->height, b->locationy + b->height);

    if (x1 < x2 && y1 < y2) {
        intersection.locationx = x1;
        intersection.locationy = y1;
        intersection.width = x2 - x1;
        intersection.height = y2 - y1;
    } else {
        intersection.locationx = 0;
        intersection.locationy = 0;
        intersection.width = 0;
        intersection.height = 0;
    }

    return intersection;
}

static bounds_t join_bounds(bounds_t* a, bounds_t* b){
    bounds_t join;

    int32_t x1 = math_min(a->locationx, b->locationx);
    int32_t y1 = math_min(a->locationy, b->locationy);
    int32_t x2 = math_max(a->locationx + a->width, b->locationx + b->width);
    int32_t y2 = math_max(a->locationy + a->height, b->locationy + b->height);

    join.locationx = x1;
    join.locationy = y1;
    join.width = x2 - x1;
    join.height = y2 - y1;

    return join;
}


static uint8_t owns_window(window_t* window) {
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

static void draw_window(window_t* window){
    graphics_write(window->buffer, window->bounds.locationx, window->bounds.locationy, window->bounds.width, window->bounds.height, window->bounds.width);
    graphics_update_region(window->bounds.locationx, window->bounds.locationy, window->bounds.width, window->bounds.height);
}

static void draw_cwindow(window_t* window, bounds_t* abs_bounds){
    bounds_t rel_bounds = relative_bounds(window, abs_bounds);
    uint32_t* buffer = window->buffer + (rel_bounds.locationy * window->bounds.width + rel_bounds.locationx);
    graphics_write(buffer, abs_bounds->locationx, abs_bounds->locationy, abs_bounds->width, abs_bounds->height, window->bounds.width);
}

static void draw_window_switch(window_t* window, bounds_t* abs_bounds){
    // Save the current CR3 value
    uint64_t current_cr3;
    asm volatile("mov %%cr3, %0" : "=r"(current_cr3));

    // Switch to the window's CR3 value
    uint64_t window_cr3 = (uint64_t) ((pcb_t*)window->owner->parent)->cr3;
    asm volatile("mov %0, %%cr3" : : "r"(window_cr3));

    draw_cwindow(window, abs_bounds);

    // Restore the original CR3 value
    asm volatile("mov %0, %%cr3" : : "r"(current_cr3));
}

static void windows_redraw(bounds_t* bounds){
    for (uint32_t i = 0; i < windows_queue->size; i++)
    {
        window_t* window = windows_queue->data[(windows_queue->head + i) % windows_queue->capacity];
        bounds_t intersection = interset_bounds(&window->bounds, bounds);
        if (intersection.width > 0 && intersection.height > 0)
            draw_window_switch(window, &intersection);
    }

    graphics_update_region(bounds->locationx, bounds->locationy, bounds->width, bounds->height);
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

    active_window = window_pntr;

    draw_window(window_pntr);
    
    return window_pntr;
}

uint8_t unregister_window(window_t* window){
    if (owns_window(window)){
        window->owner = 0;
        queue_remove(windows_queue, window);
        if (active_window == window) {
            active_window = 0;
        }
        windows_redraw(&window->bounds);
    }
    return 0;
}

uint8_t window_focus(window_t* window){
    if (owns_window(window) && active_window != window){
        queue_remove(windows_queue, window);
        queue_inque(windows_queue, window);
        active_window = window;
        draw_window(window);
    }
    return 0;
}

uint8_t update_window_bounds(window_t* window, bounds_t* bounds){
    if (owns_window(window)){
        window->bounds = *bounds;
        queue_remove(windows_queue, window);
        queue_inque(windows_queue, window);
        active_window = window;
        bounds_t joined_bounds = join_bounds(&window->bounds, bounds);
        windows_redraw(&joined_bounds);
    }
    return 0;
}

void redraw_window(window_t* window){
    bounds_t bounds;
    bounds.locationx = 0;
    bounds.locationy = 0;
    bounds.width = window->bounds.width;
    bounds.height = window->bounds.height;
    redraw_window_region(window, &bounds);
}

void redraw_window_region(window_t* window, bounds_t* bounds){
    if (owns_window(window)){
        bounds_t abs_bounds = absolute_bounds(window, bounds);
        uint8_t drawing = 0;
        for (uint32_t i = 0; i < windows_queue->size; i++)
        {
            window_t* w = windows_queue->data[(windows_queue->head + i) % windows_queue->capacity];
            if (w == window || drawing)
            {
                drawing = 1;
                bounds_t intersection = interset_bounds(&w->bounds, &abs_bounds);
                if (intersection.width > 0 && intersection.height > 0)
                    draw_window_switch(w, &intersection);
            }
        }
        graphics_update_region(abs_bounds.locationx, abs_bounds.locationy, abs_bounds.width, abs_bounds.height);
    }
}
