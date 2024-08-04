#define CHAR_WIDTH 8
#define VGA_PRINT_FG 15
#define VGA_PRINT_BG 0

#include <stdint.h>

void vga_print_char(char c, uint8_t fg, uint8_t bg);

void vga_print_str(const char* str, uint8_t* fg, uint8_t* bg);

void vga_print_str_fb(const char* str, uint8_t fg, uint8_t bg);

void vga_print_clear(uint8_t bg);

void vga_set_cursor(uint32_t offset, char c, uint8_t fg, uint8_t bg);

void vga_set_end_cursor(uint8_t fg, uint8_t bg);

void vga_set_offset(int offset);

uint32_t vga_get_offset();

#define cursor() vga_set_end_cursor(VGA_PRINT_FG, VGA_PRINT_FG)
#define newline() vga_print_char('\n', VGA_PRINT_FG, VGA_PRINT_BG)
#define print(x) vga_print_str_fb(x, VGA_PRINT_FG, VGA_PRINT_BG);cursor();
#define println(x) print(x);newline();cursor();
#define printc(x) vga_print_char(x, VGA_PRINT_FG, VGA_PRINT_BG);cursor();
#define printcln(x) printc(x);newline();cursor();