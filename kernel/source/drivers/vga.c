#include "vga.h"
#include "low_level.h"
#include "info.h"
#include "memory.h"
#include "libc.h"

uint8_t* framebuffer;
uint16_t pitch;
uint16_t width;
uint16_t height;
uint8_t* font_map;

void vga_init()
{
    vbe_mode_info_t* mode = vesa_get_mode_info();
    framebuffer = (uint8_t*)mode->framebuffer;
    pitch = mode->pitch;
    width = mode->width;
    height = mode->height;
    font_map = (uint8_t*)alloc(0x1000);
    mem_copy((char*)VESA_FONT_MAP, (char*)font_map, 0x1000);
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

void vga_draw_char(int x, int y, char c, unsigned char color) {
    uint8_t *glyph = font_map + c * FONT_HEIGHT;
    uint8_t *framepntr = framebuffer + y * pitch + x;
    for (int i = 0; i < FONT_HEIGHT; i++) {
        uint8_t line = glyph[i];
        for (int j = 0; j < 8; j++) {
            if (line & (0x80 >> j)) {
                framepntr[j +  i * pitch] = color;
            }else{
                framepntr[j +  i * pitch] = 0;
            }
        }
    }
}

void vga_copy_buffer(uint8_t *buffer)
{
    mem_copy((char*)buffer, (char*)framebuffer, pitch*height);
}

void vga_copy_image(uint8_t *image, uint32_t x, uint32_t y, uint32_t img_width, uint32_t img_height)
{
    uint8_t *framepntr = framebuffer + y * pitch + x;
    uint32_t line = img_width * pitch / width;
    for (uint32_t i = 0; i < img_height; i++)
    {
        mem_copy((char*)image, (char*)framepntr, line);
        framepntr += pitch;
        image += line;
    }
    
}
