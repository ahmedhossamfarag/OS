%macro isr 1
    global isr_%+%1
    extern %1
    isr_%+%1:
        call %1
        iret 
%endmacro

%macro isr_state 1  
    global isr_%+%1
    extern %1
    isr_%+%1:
        pusha                  ; Save all general-purpose registers
        push ds                ; Save data segment register
        push es
        push fs
        push gs
        push esp               ; Push the stack pointer to pass it to the C handler

        mov eax, 0x10
        mov ds, eax
        mov es, eax
        mov fs, eax
        mov gs, eax

        call %1     ; Call the C handler

        pop esp                ; Restore the stack pointer
        pop gs                 ; Restore segment registers
        pop fs
        pop es
        pop ds
        popa                   ; Restore general-purpose registers
        iret                   ; Return from interrupt
%endmacro

global isr_default
isr_default:
    nop
    iret

isr_state exception_handler

isr pic_handler
    
isr keyboard_handler

isr mouse_handler

isr rtc_handler

isr fpu_handler

isr page_fault_handler

isr_state timer_handler

isr_state resource_request_handler

isr resource_free_handler

isr_state lapic_timer_handler


isr_state apic_timer_handler

isr apic_keyboard_handler

isr apic_mouse_handler

isr apic_rtc_handler

isr apic_fpu_handler

isr_state schedule_thread