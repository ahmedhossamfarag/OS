.macro isr fname
    .global isr_\fname
    isr_\fname:
        call \fname
        iretq 
.endm

.macro isr_state fname 
    .global isr_\fname
    isr_\fname:
        save_regs                  # Save all general-purpose registers
        push %rsp               # Push the stack pointer to pass it to the C handler

        mov $0x10, %rax 
        mov %rax, %ds
        mov %rax, %es
        mov %rax, %fs
        mov %rax, %gs

        call \fname    # Call the C handler

        pop %rsp                # Restore the stack pointer
        restore_regs                   # Restore general-purpose registers
        iretq                   # Return from interrupt
.endm

.macro save_regs
    # Save general-purpose registers
    push %rax
    push %rcx
    push %rdx
    push %rbx
    push %rsp
    push %rbp
    push %rsi
    push %rdi

    # Save segment registers
    mov %gs, %rax
    push %rax
    mov %fs, %rax
    push %rax
    mov %es, %rax
    push %rax
    mov %ds, %rax
    push %rax
.endm

.macro restore_regs
    # Restore segment registers
    pop %rax
    mov %rax, %ds
    pop %rax
    mov %rax, %es
    pop %rax
    mov %rax, %fs
    pop %rax
    mov %rax, %gs

    # Restore general-purpose registers
    pop %rdi
    pop %rsi
    pop %rbp
    pop %rsp
    pop %rbx
    pop %rdx
    pop %rcx
    pop %rax
.endm

.macro isr_error fname 
    .global isr_\fname
    isr_\fname:
        call \fname    # Call the C handler
        add $8, %rsp                # Adjust the stack pointer to remove the error code
        iretq                   # Return from interrupt
.endm

.global isr_default
isr_default:
    nop
    iretq

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

isr_error error_exception_handler

isr_error gp_fault_handler