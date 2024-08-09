[ org 0x7c00 ]

%include "define.asm"

mov [ BOOT_DRIVE ] , dl ; BIOS stores our boot drive in DL

mov bp , REAL_MODE_SP ; Set - up the stack.
mov sp , bp

%include "info.asm"

%include "vesa.asm"

%include "kernel_load.asm"

%include "switch_to_pm.asm"

halt:
jmp $

; data
BOOT_DRIVE db 0
%include "gdt.asm"

%include "ap_setup.asm"

; Bootsector padding
times 510 -( $ - $$ ) db 0
dw 0xaa55
