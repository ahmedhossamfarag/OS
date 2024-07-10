#define VideoMemoryStart 0xB8000
#define VideoMemoryEndOffset 2000
#define VideoMemoryLineLn 80
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

void screen_scroll_up();

void screen_new_line();

void screen_print(char c);

void screen_print_char(char c);

void screen_print_str(const char* str);

void screen_clear();

void screen_set_cursor(int offset);

int screen_get_cursor();


#define print(x) screen_print_str(x)
#define println(x) print(x);screen_print_char('\n')
#define printc(x) screen_print_char(x)
#define printcln(x) printc(x);screen_print_char('\n')