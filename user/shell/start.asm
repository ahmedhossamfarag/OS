extern main
global _start
_start:
    call main
    mov esi, 0xD
    int 0x80
    jmp $