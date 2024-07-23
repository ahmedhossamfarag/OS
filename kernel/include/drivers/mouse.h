#define PS2_CMD_PORT 0x64
#define PS2_DATA_PORT 0x60


#define LeftButton 0x01
#define RightButton 0x02
#define MiddleButton 0x04
#define XMoveSign 0x10
#define YMoveSign 0x20
#define XOverflow 0x40
#define YOverflow 0x80

#define MouseDataPort 0x60

#define MouseIRQ 12 

#define MaxMouseX 2000
#define MaxMouseY 1600

#include <stdint.h>

typedef struct{
    uint8_t left_button;
    uint8_t right_button;
    uint8_t middle_button;
    int16_t delta_x;
    int16_t delta_y;
    uint16_t mouse_x;
    uint16_t mouse_y;
} mouse_info_t;


void mouse_handler();

void set_mouse_handler_proc(void (*proc)(mouse_info_t));

void mouse_init();

void apic_mouse_handler();