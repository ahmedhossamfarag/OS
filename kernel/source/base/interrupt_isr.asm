extern exception_handler
extern pic_handler
extern timer_handler
extern keyboard_handler
extern mouse_handler
extern rtc_handler
extern fpu_handler
extern page_fault_handler

global isr_exception_handler
global isr_pic_handler
global isr_timer_handler
global isr_keyboard_handler
global isr_default
global isr_mouse_handler
global isr_rtc_handler
global isr_fpu_handler
global isr_page_fault_handler

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

isr_mouse_handler:
    call mouse_handler
    iret

isr_rtc_handler:
    call rtc_handler
    iret

isr_fpu_handler:
    call fpu_handler
    iret

isr_page_fault_handler:
    call page_fault_handler
    iret

isr_default:
    nop
    iret