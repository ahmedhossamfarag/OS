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
    cmp cx, VESA_DESIRED_MODE
    je vesa_set
    cmp cx, 0xFFFF
    je vesa_error
    add si, 2
    jmp vesa_mode_loop
vesa_set:
    mov [VESA_MODE], cx
    mov ax, 0
    mov es, ax
    mov di, VESA_MODE_INFO
    mov ax, 0x4F01
    int 0x10
    cmp ax, 0x004F
    jne vesa_error
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