#define VideoMemoryStart 0xB8000
#define VideoMemoryEndOffset 2000
#define VideoMemoryLineLn 80
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

void screen_print(int offset, char c);

void screen_print_str(int offset, char* str);

void screen_clear(int offset);

void screen_set_cursor(int offset);

int screen_get_cursor();
