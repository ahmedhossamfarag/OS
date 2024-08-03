#define CHAR_WIDTH 8
#define VGA_PRINT_COLOR 15

void vga_scroll_up();

void vga_new_line();

void vga_print(char c);

void vga_print_char(char c);

void vga_print_str(const char* str);

void vga_print_clear();

void vga_set_cursor(int offset);

int vga_get_cursor();


#define print(x) vga_print_str(x)
#define println(x) print(x);vga_print_char('\n')
#define printc(x) vga_print_char(x)
#define printcln(x) printc(x);vga_print_char('\n')