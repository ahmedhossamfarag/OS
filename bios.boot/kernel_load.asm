[bits 16]

mov bx , KERNEL_LOAD_OFFSET
mov dh , KERNEL_N_SECTORS
mov dl , [ BOOT_DRIVE ]

push dx
mov ah , 0x02 ; BIOS read sector function
mov al , dh
mov ch , KERNEL_SELECT_CYLINDER
mov dh , KERNEL_SELECT_HEAD
mov cl , KERNEL_START_SECTOR
int 0x13 ; BIOS interrupt
pop dx

; jc disk_error ; Jump if error ( i.e. carry flag set )
; cmp dh , al ; if AL ( sectors read ) != DH ( sectors expected )
; jne disk_error ; display error message
jmp load_end

disk_error :
jmp $

load_end:
nop