; .macro isr fname
;     .global isr_\fname
;     isr_\fname:
;         call \fname
;         iretq 
; .endm

.macro isr_state fname 
    .global isr_\fname
    isr_\fname:
        save_regs                  # Save all general-purpose registers
        mov %rsp, %rdi          # Pass the stack pointer as an argument to the C handler

        mov $0x20, %rax         # Set the data segment selector (0x20) for the kernel data segment
        mov %rax, %ds
        mov %rax, %es
        mov %rax, %fs
        mov %rax, %gs

        leaq \fname(%rip), %rax # Get the address of the C handler
        call *%rax              # Call the C handler

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

; .macro isr_error fname 
;     .global isr_\fname
;     isr_\fname:
;         call \fname    # Call the C handler
;         add $8, %rsp                # Adjust the stack pointer to remove the error code
;         iretq                   # Return from interrupt
; .endm

.macro isr_state_error fname
    .global isr_\fname
    isr_\fname:
        save_regs                  # Save all general-purpose registers
        mov %rsp, %rdi          # Pass the stack pointer as an argument to the C handler
        leaq \fname(%rip), %rax # Get the address of the C handler
        call *%rax              # Call the C handler
        restore_regs                   # Restore general-purpose registers
        add $8, %rsp                # Adjust the stack pointer to remove the error code
        iretq                   # Return from interrupt
.endm

.global isr_default
isr_default:
    nop
    iretq



;# X86 Interrupt Service Routines

isr_state exception_handler

isr_state divide_error_handler

isr_state debug_exception_handler

isr_state nmi_handler

isr_state breakpoint_handler

isr_state overflow_handler

isr_state bound_range_exceeded_handler

isr_state invalid_opcode_handler

isr_state device_not_available_handler

isr_state_error double_fault_handler

isr_state coprocessor_segment_overrun_handler

isr_state_error invalid_TSS_handler

isr_state_error segment_not_present_handler

isr_state_error stack_segment_fault_handler

isr_state_error general_protection_fault_handler

isr_state_error page_fault_handler

isr_state x87_floating_point_handler

isr_state_error alignment_check_handler

isr_state machine_check_handler

isr_state simd_exception_handler

isr_state virtualization_exception_handler

;# PIC Interrupt Service Routines

isr_state pic_handler
    
isr_state timer_handler

isr_state keyboard_handler

isr_state rtc_handler

isr_state mouse_handler

isr_state fpu_handler

isr_state ata_handler

isr_state lapic_timer_handler

;# APIC Interrupt Service Routines

isr_state apic_timer_handler

isr_state apic_keyboard_handler

isr_state apic_rtc_handler

isr_state apic_mouse_handler

isr_state apic_fpu_handler

isr_state apic_ata_handler

;# Scheduler Interrupt Service Routines

isr_state schedule_thread

isr_state schedule_process_waiting

isr_state schedule_thread_waiting

isr_state schedule_process_terminated

isr_state schedule_thread_terminated

;# System Call Interrupt Service Routines

isr_state syscall_handler
