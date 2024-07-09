#define VideoMemoryStart 0xB8000
#define VideoMemoryEndOffset 2000
#define VideoMemoryLineLn 80
#define VideoMemoryEndLine 25
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

void screen_edit_init();

void screen_edit_print_char(char c);

void screen_edit_print_str(const char* str);

void screen_edit_del_before();

void screen_edit_del_after();

void screen_edit_cursor_up();

void screen_edit_cursor_down();

void screen_edit_cursor_left();

void screen_edit_cursor_right();

void screen_edit_new_line();

void screen_edit_clear();