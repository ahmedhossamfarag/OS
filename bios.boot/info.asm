; get memory map
mov si, 0
mov eax, 0xE820
mov edx, 0x534D4150  ; "SMAP"
mov ecx, 24          ; Size of the buffer (24 bytes)
mov ebx, 0           ; Continuation value, initially 0
mov di, MEMORY_MAP       ; ES:DI points to the buffer

next_entry:
    int 0x15
    jc ms_error             ; Jump if carry flag is set (error)
    cmp eax, 0x534D4150  ; Check if EAX was overwritten with the signature
    jne done             ; If not, we are done

    inc si               ; Inc no of maps
    add di, 24
    ; Check if more entries are available
    test ebx, ebx
    jnz next_entry

done:
    mov [MEMORY_N_MAP] , si

ms_error:
    nop

; get disk size
; mov ax, 0x4800
; mov dl, [ BOOT_DRIVE ]  ; Drive number (0x80 for first hard disk)
; int 0x13 ; If successful, CX:DX will contain the total number of sectors
; jc ds_error
; mov [DISk_SIZE] , dx
; mov [DISk_SIZE+2] , cx
; ds_error:
;     nop