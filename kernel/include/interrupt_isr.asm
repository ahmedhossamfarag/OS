extern exception_handler
extern pic_handler
extern timer_handler
extern keyboard_handler

global isr_exception_handler
global isr_pic_handler
global isr_timer_handler
global isr_keyboard_handler
global isr_default


isr_exception_handler:
    call exception_handler
    iret

isr_pic_handler:
    call pic_handler
    iret

    
isr_timer_handler:
    call timer_handler
    iret
    
isr_keyboard_handler:
    call keyboard_handler
    iret

isr_default:
    nop
    iret