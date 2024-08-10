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

%include "ap_startf.asm"
