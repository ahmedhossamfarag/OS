#include "vga.h"
#include "low_level.h"
#include "info.h"

uint8_t* framebuffer;
uint16_t pitch;
uint16_t width;
uint16_t height;

void vga_init()
{
    vbe_mode_info_t* mode = vesa_get_mode_info();
    framebuffer = (uint8_t*)mode->framebuffer;
    pitch = mode->pitch;
    width = mode->width;
    height = mode->height;
}

uint8_t vga_available()
{
    return vesa_get_mode() != 0xFFFF;
}

void vga_set_palette_entry(uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    outb(VGA_PALETTE_INDEX, index);  // Set the palette index
    outb(VGA_PALETTE_DATA, red);     // Set the red component
    outb(VGA_PALETTE_DATA, green);   // Set the green component
    outb(VGA_PALETTE_DATA, blue);    // Set the blue component
}

void vga_clear(uint8_t color)
{
    uint32_t len = width * height;
    for (uint8_t* pixel = framebuffer; pixel < framebuffer + len; pixel++)
    {
        *pixel = color;
    }
}
