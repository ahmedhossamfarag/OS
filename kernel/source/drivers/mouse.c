#include "mouse.h"
#include "pic.h"
#include "low_level.h"
#include "apic.h"

int16_t MouseX;
int16_t MouseY;


static void update_mouse_location(int16_t delta_x, int16_t delta_y){

    MouseX += delta_x;
    MouseY += delta_y;

    if (MouseX < 0) MouseX = 0;
    if (MouseY < 0) MouseY = 0;

    if (MouseX > MaxMouseX) MouseX = MaxMouseX;
    if(MouseY > MaxMouseY) MouseY = MaxMouseY;
    
}


static void (*mouse_handler_proc)(MouseInfo);

static void handle_mouse_info(){
    MouseInfo info;

    uint8_t status = inb(MouseDataPort); // Read status byte from the PS/2 controller

    uint8_t delta_x = inb(MouseDataPort); // Second byte (X movement)
    uint8_t delta_y = inb(MouseDataPort); // Third byte (Y movement)

    // Process Data

    info.left_button = status & LeftButton  != 0;
    info.right_button = status & RightButton != 0;
    info.middle_button = status & MiddleButton != 0;

    info.delta_x = status & XMoveSign == 0 ? delta_x : -delta_x;
    info.delta_y = status & YMoveSign == 0 ? delta_y : -delta_y;

    update_mouse_location(info.delta_x, info.delta_y);

    info.mouse_x = MouseX;
    info.mouse_y = MouseY;

    // Excute Proc
    if(mouse_handler_proc)
        mouse_handler_proc(info);
}

/* IRQ 12 */
void mouse_handler() {

    handle_mouse_info();

    // Send End of Interrupt (EOI) signal to the PIC
    pic_sendEOI(MouseIRQ);
}

void set_mouse_handler_proc(void (*proc)(MouseInfo))
{
    mouse_handler_proc = proc;
}



static void enable_mouse_device() {
    // Enable the auxiliary device - the mouse
    outb(PS2_CMD_PORT, 0xA8);
    // Enable IRQ12 (unmask it)
    outb(PS2_CMD_PORT, 0x20); // Command to read the command byte
    uint8_t status = inb(PS2_DATA_PORT);
    status |= 0x02;
    outb(PS2_CMD_PORT, 0x60); // Command to write the command byte
    outb(PS2_DATA_PORT, status);

    // Enable mouse interrupt
    outb(PS2_CMD_PORT, 0xD4);
    outb(PS2_DATA_PORT, 0xF4);
}

void initialize_mouse() {
    enable_mouse_device();
}


/* IRQ 12 */
void apic_mouse_handler() {

    handle_mouse_info();

    // Send End of Interrupt (EOI) signal to the PIC
    apic_sendEOI();
}