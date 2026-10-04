#ifndef WINLIB_H
#define WINLIB_H


#include "winlib.h"
#include "memlib.h"
#include "syscall_map.h"

window_t* create_window(int32_t x, int32_t y, uint32_t width, uint32_t height, uint32_t bgcolor)
{
    window_t* window = (window_t*)malloc(sizeof(window_t));
    window->bounds.locationx = x;
    window->bounds.locationy = y;
    window->bounds.width = width;
    window->bounds.height = height;
    window->buffer = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    for (uint32_t* pixel = window->buffer; pixel < window->buffer + width * height; pixel++)
    {
        *pixel = bgcolor;
    }
    return window;
}

window_t* register_window(window_t* window)
{
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(REGISTER_WINDOW_SYSCALL),"d"((uint64_t)window));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (window_t*)result;
}

uint8_t deregister_window(window_t* window)
{
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(UNREGISTER_WINDOW_SYSCALL),"d"((uint64_t)window));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t redraw_window(window_t* window)
{
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(REDRAW_WINDOW_SYSCALL),"d"((uint64_t)window));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t update_window_bounds(window_t* window, bounds_t* bounds)
{
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(UPDATE_WINDOW_BOUNDS_SYSCALL),"d"((uint64_t)window),"c"((uint64_t)bounds));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

#endif // !WINLIB_H
