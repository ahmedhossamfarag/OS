; kernel

KERNEL_LOAD_OFFSET equ 0x1000
KERNEL_OFFSET equ 0xA000000

; kernel load

KERNEL_N_SECTORS equ 50
KERNEL_SELECT_CYLINDER equ 0
KERNEL_SELECT_HEAD equ 0
KERNEL_START_SECTOR equ 2

; video memory

VIDEO_MEMORY equ 0xB8000
WHITE_ON_BLACK equ 0x0F

; stack pointer

REAL_MODE_SP equ 0x9000
PROTECTED_MODE_SP equ 0xB000000

; info
MEMORY_N_MAP equ 0x8000
MEMORY_MAP equ 0x8004

VESA_DESIRED_MODE equ 0x0103
VESA_INFO equ 0xC000
VESA_MODE equ 0xD000
VESA_MODE_INFO equ 0xD100
VESA_FONT_MAP equ 0xE000