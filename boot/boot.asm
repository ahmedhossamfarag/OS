[ org 0x7c00 ]

%include "boot/define.asm"

mov [ BOOT_DRIVE ] , dl ; BIOS stores our boot drive in DL

mov bp , REAL_MODE_SP ; Set - up the stack.
mov sp , bp

mov bx , MSG_REAL_MODE
call print_string

%include "boot/kernel_load.asm"

%include "boot/switch_to_pm.asm"

halt:
jmp $

%include "boot/boot_data.asm"
%include "boot/print_string.asm"
%include "boot/gdt.asm"


; Bootsector padding
times 510 -( $ - $$ ) db 0
dw 0xaa55
