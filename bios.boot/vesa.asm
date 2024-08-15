mov ax, 0
mov es, ax
mov di, VESA_INFO
mov ax, 0x4F00
int 0x10
cmp ax, 0x004F
jne vesa_error
mov si, [VESA_INFO+14]
vesa_mode_loop:
    mov cx, [si]
    cmp cx, 0xFFFF
    je vesa_error

    ; check width
    call vesa_mode_info
    mov dx, [VESA_MODE_INFO+18]
    cmp dx, VESA_DESIRED_WIDTH
    jle vesa_next_mode
    ; check height
    mov dx, [VESA_MODE_INFO+20]
    cmp dx, VESA_DESIRED_HEIGHT
    jle vesa_next_mode
    ; check depth
    mov dl, [VESA_MODE_INFO+25]
    cmp dl, VESA_DESIRED_DEPTH
    jne vesa_next_mode
    ; check framebuffer
    mov dx, [VESA_MODE_INFO+42]
    cmp dx, 0
    je vesa_next_mode

    jmp vesa_set

    vesa_next_mode:
        add si, 2
        jmp vesa_mode_loop

vesa_mode_info:
    mov ax, 0
    mov es, ax
    mov di, VESA_MODE_INFO
    mov ax, 0x4F01
    int 0x10
    cmp ax, 0x004F
    jne vesa_error
    ret

vesa_set:
    mov [VESA_MODE], cx
vesa_mode_set:
    mov bx, cx
    or bx, 0x4000
    mov ax, 0x4F02
    int 0x10
    cmp ax, 0x004F
    jne vesa_error
    jmp vesa_end

vesa_error:
    mov cx, 0xFFFF
    mov [VESA_MODE], cx
    nop


vesa_end:
    nop