.macro isr fname
    .global isr_\fname
    isr_\fname:
        call \fname
        iret 
.endm

.macro isr_state fname 
    .global isr_\fname
    isr_\fname:
        pusha                  # Save all general-purpose registers
        push %ds                # Save data segment register
        push %es
        push %fs
        push %gs
        push %esp               # Push the stack pointer to pass it to the C handler

        mov $0x10, %eax 
        mov %eax, %ds
        mov %eax, %es
        mov %eax, %fs
        mov %eax, %gs

        call \fname    # Call the C handler

        pop %esp                # Restore the stack pointer
        pop %gs                 # Restore segment registers
        pop %fs
        pop %es
        pop %ds
        popa                   # Restore general-purpose registers
        iret                   # Return from interrupt
.endm

.global isr_default
isr_default:
    nop
    iret


isr_state exception_handler

isr pic_handler
    
isr keyboard_handler

isr mouse_handler

isr rtc_handler

isr fpu_handler

isr ata_handler

isr page_fault_handler

isr_state timer_handler

isr_state lapic_timer_handler


isr_state apic_timer_handler

isr apic_keyboard_handler

isr apic_mouse_handler

isr apic_rtc_handler

isr apic_fpu_handler

isr apic_ata_handler

isr_state schedule_thread

isr_state schedule_process_waiting

isr_state schedule_thread_waiting

isr_state schedule_process_terminated

isr_state schedule_thread_terminated

isr_state syscall_handler
