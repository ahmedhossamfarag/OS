AP_SETUP_OFFSET equ 0xA000

[bits 16]
ap_setup_start:
    cli ; switch off interrupts

    lgdt [ _gdt_descriptor - ap_setup_start + AP_SETUP_OFFSET ] ; Load our global descriptor table , which defines the protected mode segments ( e.g. for code and data )

    mov eax , cr0 ; To make the switch to protected mode , we set cr0 first bit to 1
    or eax , 0x1
    mov cr0 , eax

    jmp 0x8 : ap_init_pm - ap_setup_start + AP_SETUP_OFFSET ; Make a far jump 

    [bits 32]
    ap_init_pm :
        mov ax , 0x10 ;
        mov ds , ax
        mov ss , ax
        mov es , ax
        mov fs , ax
        mov gs , ax

        ; Load PML4
        mov eax, [AP_SETUP_OFFSET + 0x300]
        mov cr3, eax

        ; Enable PAE
        mov eax, cr4
        or  eax, 1 << 5          ; CR4.PAE
        mov cr4, eax

        ; Enable Long Mode Enable (LME)
        mov ecx, 0xC0000080      ; IA32_EFER
        rdmsr
        or  eax, 1 << 8          ; EFER.LME
        wrmsr

        ; Enable paging
        mov eax, cr0
        or  eax, 1 << 31         ; CR0.PG
        mov cr0, eax

        ; Enter 64-bit code segment
        jmp 0x18: long_mode_entry - ap_setup_start + AP_SETUP_OFFSET ; Make a far jump


        ; -----------------------------
        ; 64-bit long mode
        ; -----------------------------

        [bits 64]

        long_mode_entry:
                mov ax, 0x20
                mov ds, ax
                mov es, ax
                mov ss, ax
                mov fs, ax
                mov gs, ax

                mov rbp , [AP_SETUP_OFFSET + 0x100] ; Update our stack position so it is right
                mov rsp , rbp


                mov rbx, [AP_SETUP_OFFSET + 0x200]
                call rbx ; Finally , call the kernel
                jmp $


_gdt_start:

_gdt_null:
    dd 0x00000000
    dd 0x00000000

_gdt_code:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10011010b       ; Present, ring 0, code, readable
    db 11001111b       ; G=1, D=1, L=0
    db 0x00

_gdt_data:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10010010b       ; Present, ring 0, data, writable
    db 11001111b       ; G=1, D=1
    db 0x00

_gdt_code64:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10011010b       ; Present, ring 0, code, readable
    db 10101111b       ; G=1, L=1, D=0
    db 0x00

_gdt_data64:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10010010b       ; Present, ring 0, data, writable
    db 00001111b       ; G=0, L=0, D=0
    db 0x00

_gdt_end:

_gdt_descriptor:
    dw _gdt_end - _gdt_start - 1
    dd _gdt_start - ap_setup_start + AP_SETUP_OFFSET

ap_setup_end:
