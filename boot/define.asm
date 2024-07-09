; kernel

KERNEL_OFFSET equ 0x1000

; kernel load

KERNEL_N_SECTORS equ 50
KERNEL_SELECT_CYLINDER equ 0
KERNEL_SELECT_HEAD equ 0
KERNEL_START_SECTOR equ 2

; video memory

VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f

; stack pointer

REAL_MODE_SP equ 0x9000
PROTECTED_MODE_SP equ 0x90000

