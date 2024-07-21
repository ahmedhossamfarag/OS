[bits 32]
ap_setup_shift:
    mov esi , ap_setup_start
    mov edi , 0xA000
    mov ecx , ap_setup_end - ap_setup_start
    setup_shift:
        mov al , [esi]
        mov [edi] , al
        add esi , 1
        add edi , 1
        loop setup_shift
    ret

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

        mov ebp , [0xA100] ; Update our stack position so it is right
        mov esp , ebp


        mov ebx, [0xA200]
        call ebx ; Finally , call the kernel
        jmp $
ap_setup_end:
