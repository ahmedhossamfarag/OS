[ org 0x7c00 ]

%include "define.asm"

mov [ BOOT_DRIVE ] , dl ; BIOS stores our boot drive in DL

mov bp , REAL_MODE_SP ; Set - up the stack.
mov sp , bp

%include "info.asm"

; %include "vesa.asm"

%include "disk_read.asm"

%include "switch_to_pm.asm"

halt:
jmp $

; data
%include "boot_data.asm"
%include "gdt.asm"

; Bootsector padding
times 510 -( $ - $$ ) db 0
dw 0xaa55
