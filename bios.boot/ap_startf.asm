AP_SETUP_OFFSET equ 0xA000

[bits 16]
ap_setup_start:
    cli ; switch off interrupts

    lgdt [ _gdt_descriptor - ap_setup_start + AP_SETUP_OFFSET ] ; Load our global descriptor table , which defines the protected mode segments ( e.g. for code and data )

    mov eax , cr0 ; To make the switch to protected mode , we set cr0 first bit to 1
    or eax , 0x1
    mov cr0 , eax

    jmp 0x8 : ap_init_pm - ap_setup_start + AP_SETUP_OFFSET ; Make a far jump 

    [ bits 32]
    ap_init_pm :
        mov ax , 0x10 ;
        mov ds , ax
        mov ss , ax
        mov es , ax
        mov fs , ax
        mov gs , ax

        mov ebp , [AP_SETUP_OFFSET + 0x100] ; Update our stack position so it is right
        mov esp , ebp


        mov ebx, [AP_SETUP_OFFSET + 0x200]
        call ebx ; Finally , call the kernel
        jmp $


_gdt_start :
_gdt_null : 
    dd 0x0 
    dd 0x0
_gdt_code :
    dw 0xffff
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
_gdt_data :
    dw 0xffff 
    dw 0x0 
    db 0x0 
    db 10010010b 
    db 11001111b 
    db 0x0 
_gdt_end :
_gdt_descriptor :
    dw _gdt_end - _gdt_start - 1
    dd _gdt_start- ap_setup_start + AP_SETUP_OFFSET

ap_setup_end:
