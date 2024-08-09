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
vesa_mode_info:
    mov ax, 0
    mov es, ax
    mov di, VESA_MODE_INFO
    mov ax, 0x4F01
    int 0x10
    cmp ax, 0x004F
    jne vesa_error
vesa_mode_set:
    mov bx, cx
    or bx, 0x4000
    mov ax, 0x4F02
    int 0x10
    cmp ax, 0x004F
    jne vesa_error
vesa_font_get:
    push ds
	push es
	mov	ax, 1130h
	mov	bh, 6
	int	10h
	push es
	pop	ds
	pop	es
	mov	si, bp
    mov di, VESA_FONT_MAP
	mov	cx, 256*16/4
	rep	movsd
	pop	ds
    jmp vesa_end
vesa_error:
    mov cx, 0xFFFF
    mov [VESA_MODE], cx
    nop
vesa_end:
    nop