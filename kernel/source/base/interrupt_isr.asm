%macro isr 1
    global isr_%+%1
    extern %1
    isr_%+%1:
        call %1
        iret 
%endmacro


isr_default:
    nop
    iret

isr exception_handler

isr pic_handler
    
isr keyboard_handler

isr mouse_handler

isr rtc_handler

isr fpu_handler

isr page_fault_handler

global isr_timer_handler
extern timer_handler
isr_timer_handler:
    pusha                  ; Save all general-purpose registers
    push ds                ; Save data segment register
    push es
    push fs
    push gs
    push esp               ; Push the stack pointer to pass it to the C handler

    call timer_handler     ; Call the C handler

    pop esp                ; Restore the stack pointer
    pop gs                 ; Restore segment registers
    pop fs
    pop es
    pop ds
    popa                   ; Restore general-purpose registers
    iret                   ; Return from interrupt
