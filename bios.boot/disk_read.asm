[bits 16]
push es                         ; save es

get_disk_params:
    mov ah , 0x08
    mov dl , [BOOT_DRIVE]
    int 0x13
    mov [DISK_SPT] , cl

set_load_offset:
    mov ax , KERNEL_LOAD_OFFSET / 16
    mov es , ax

set_n_sector:
    mov ax , [DISK_LD_SECTOR]
    test ax , ax
    jz load_end
    xor cx , cx
    xor dx , dx
    mov cl , [DISK_SPT]
    mov dl , [DISK_ST_SECTOR]
    sub cl , dl
    add cl , 1
    cmp ax , cx
    jle set_remain
    mov [DISK_N_SECTOR] , cl
    jmp read_disk
    set_remain:
        mov [DISK_N_SECTOR] , al

read_disk:
    mov ah , 0x02 ; BIOS read sector function
    mov bx , 0
    mov dl , [BOOT_DRIVE]
    mov ch , [DISK_CYLINDER]
    mov al , [DISK_N_SECTOR]
    mov dh , [DISK_HEAD]
    mov cl , [DISK_ST_SECTOR]
    int 0x13 ; BIOS interrupt

update_params:
    mov al , 1
    mov [DISK_ST_SECTOR] , al   ; set start sector to 0
    mov al , [DISK_HEAD] 
    add al , 1
    mov [DISK_HEAD] , al        ; add 1 to head 
    mov ax , [DISK_LD_SECTOR]
    xor cx , cx
    mov cl , [DISK_N_SECTOR]
    sub ax , cx
    mov [DISK_LD_SECTOR] , ax   ; sub loaded sectors
    shl cx , 5
    mov ax , es
    add ax , cx
    mov es , ax                 ; update offset

jmp set_n_sector

load_end:
    pop es                      ; reset es