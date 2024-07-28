extern fopen_handler
extern fsize_handler
extern fclose_handler
extern fread_handler
extern fwrite_handler
extern fcreate_handler
extern fdelete_handler
extern flist_handler

extern process_create_handler
extern thread_create_handler
extern process_exit_handler
extern thread_exit_handler
extern process_terminate_handler
extern thread_terminate_handler
extern memory_init_handler

extern prints_handler
extern scans_handler
extern scans_line_handler

syscall_map:
    dd 0
    dd fopen_handler ;0x1
    dd fsize_handler ;0x2
    dd fclose_handler ;0x3
    dd fread_handler ;0x4
    dd fwrite_handler ;0x5
    dd fcreate_handler ;0x6
    dd fdelete_handler ;0x7
    dd flist_handler ;0x8
    dd 0 ;0x9
    dd process_create_handler ;0xA
    dd thread_create_handler ;0xB
    dd process_exit_handler ;0xC
    dd thread_exit_handler ;0xD
    dd process_terminate_handler ;0xE
    dd thread_terminate_handler ;0xF
    dd prints_handler ;0x10
    dd scans_handler ;0x11
    dd scans_line_handler ;0x12
    dd 0 ;0x13
    dd memory_init_handler ;0x14
    dd 0 ;0x15
    
global isr_syscall_handler
isr_syscall_handler:
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

    shl esi, 2
    call [syscall_map+esi]

    pop esp                ; Restore the stack pointer
    pop gs                 ; Restore segment registers
    pop fs
    pop es
    pop ds
    popa                   ; Restore general-purpose registers
    iret               