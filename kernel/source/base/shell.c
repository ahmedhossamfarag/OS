#include "shell.h"
#include "screen_edit.h"

void shell_init()
{
	screen_edit_init();
	screen_edit_clear();
	set_keyboard_handler_proc(shell_keyboard_handler);
}

void shell_end_listen()
{
	set_keyboard_handler_proc(0);
}

void shell_keyboard_handler(KeyInfo info)
{
	if (info.key_release) {
		if (info.character) {
			screen_edit_print_char(info.character);
		}
		else
		{
			switch (info.key_code)
			{
			case ENTER:
				screen_edit_new_line();
				break;
			case SPACE:
				screen_edit_print_char(' ');
				break;
			case BACKSPACE:
				screen_edit_del_before();
				break;
			case DEL:
				screen_edit_del_after();
				break;
			case UP:
				screen_edit_cursor_up();
				break;
			case DOWN:
				screen_edit_cursor_down();
				break;
			case LEFT:
				screen_edit_cursor_left();
				break;
			case RIGHT:
				screen_edit_cursor_right();
				break;
			default:
				break;
			}
		}
	}
}
