#include "vga_edit.h"
#include "vga_print.h"
#include "keyboard.h"
#include "memory.h"

static char* edit_des;
static uint32_t edit_len;
static uint32_t edit_indx;
static uint32_t des_len;
static uint32_t vga_ofs;
static void (*edit_end_proc)(char*);

static void keyboard_proc(key_info_t);

void vga_edit_init(uint32_t len, void (*end_proc)(char*))
{
    if(!len){
        if(end_proc) end_proc(0);
        return;
    }

    edit_des = alloc(len);
    if(!edit_des){
        if(end_proc) end_proc(0);
        return;
    }

    des_len = len;
    edit_end_proc = end_proc;
    edit_len = edit_indx = edit_des[0] = 0;
    vga_ofs = vga_get_offset();
    set_keyboard_handler_proc(keyboard_proc);
}

static void vga_edit_end(){
    set_keyboard_handler_proc(0);
    vga_set_cursor(vga_ofs + edit_indx, edit_des[edit_indx], VGA_PRINT_FG, VGA_PRINT_BG);
    printc('\n');
    if(edit_end_proc){
        edit_end_proc(edit_des);
    }
    free(edit_des, des_len);
}

static void vga_edit_left(){
    if(edit_indx > 0){
        if(edit_indx == edit_len){
            vga_set_cursor(vga_ofs + edit_indx, 0, VGA_PRINT_BG, VGA_PRINT_BG);
        }else{
            vga_set_cursor(vga_ofs + edit_indx, edit_des[edit_indx], VGA_PRINT_FG, VGA_PRINT_BG);
        }
        edit_indx --;
        vga_set_cursor(vga_ofs + edit_indx, edit_des[edit_indx], VGA_PRINT_BG, VGA_PRINT_FG);
    }
}

static void vga_edit_right(){
    if(edit_indx < edit_len){
        vga_set_cursor(vga_ofs + edit_indx, edit_des[edit_indx], VGA_PRINT_FG, VGA_PRINT_BG);
        edit_indx ++;
        if(edit_indx == edit_len){
            vga_set_cursor(vga_ofs + edit_indx, 0, VGA_PRINT_FG, VGA_PRINT_FG);
        }else{
            vga_set_cursor(vga_ofs + edit_indx, edit_des[edit_indx], VGA_PRINT_BG, VGA_PRINT_FG);
        }
    }
}

static void vga_edit_catch_char(char c){
    edit_des[edit_indx] = c;

    if(edit_indx == edit_len){
        printc(c);
        edit_indx ++;
        edit_len ++;

        if(edit_len == des_len){
            vga_edit_end();
        }else{
            edit_des[edit_indx] = 0;
        }

    }else{
        vga_edit_right();
    }
}

static void vga_edit_back(){
    if(edit_indx > 0){
        if(edit_indx == edit_len){

            vga_set_cursor(vga_ofs + edit_indx, 0, VGA_PRINT_BG, VGA_PRINT_BG);
            edit_len --;
            edit_indx --;
            edit_des[edit_indx] = 0;
            vga_set_cursor(vga_ofs + edit_indx, 0, VGA_PRINT_FG, VGA_PRINT_FG);

        }else{

            for (uint32_t i = edit_indx; i < edit_len; i++)
            {
                edit_des[i] = edit_des[i+1];
            }
            edit_des[edit_len] = 0;

            vga_set_cursor(vga_ofs + edit_indx, edit_des[edit_indx], VGA_PRINT_BG, VGA_PRINT_FG);
            for (uint32_t i = edit_indx + 1; i < edit_len; i++){
                vga_set_cursor(vga_ofs + i, edit_des[i], VGA_PRINT_FG, VGA_PRINT_BG);
            }

            edit_len --;
        }

        vga_set_offset(vga_ofs + edit_len);
    }
}

static void keyboard_proc(key_info_t k){
    if(!k.key_release) return;

    switch (k.key_code)
    {
    case CHAR:
        vga_edit_catch_char(k.character);
        break;
    case SPACE:
        vga_edit_catch_char(' ');
        break;
    case ENTER:
        vga_edit_end();
        break;
    case LEFT:
        vga_edit_left();
        break;
    case RIGHT:
        vga_edit_right();
        break;
    case BACKSPACE:
        vga_edit_back();
        break;
    default:
        break;
    }
}