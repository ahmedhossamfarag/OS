[org 0xE000] 
[bits 16]
ap_setup_start:
    cli ; switch off interrupts

    lgdt [ gdt_descriptor ] ; Load our global descriptor table , which defines the protected mode segments ( e.g. for code and data )

    mov eax , cr0 ; To make the switch to protected mode , we set cr0 first bit to 1
    or eax , 0x1
    mov cr0 , eax

    jmp CODE_SEG : ap_init_pm ; Make a far jump 

   [ bits 32]
    ap_init_pm :
        mov ax , DATA_SEG ;
        mov ds , ax
        mov ss , ax
        mov es , ax
        mov fs , ax
        mov gs , ax

        mov ebp , [0xA0000]
        mov esp , ebp


        mov ebx, [0xB0000]
        call ebx
        jmp $
ap_setup_end:

gdt_start :
gdt_null : 
    dd 0x0 
    dd 0x0
gdt_code :
    dw 0xffff
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data :
    dw 0xffff 
    dw 0x0 
    db 0x0 
    db 10010010b 
    db 11001111b 
    db 0x0 
gdt_end :
gdt_descriptor :
    dw gdt_end - gdt_start - 1
    dd gdt_start
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

