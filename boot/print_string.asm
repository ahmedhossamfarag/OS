[bits 16]

;print string start @ offset bx
print_string:
    pusha
    
    mov ah , 0x0e
    print:
    mov al, [bx]
    cmp al, 0 ;0 end of string
    je end
    int 0x10 ;BIOS output
    inc bx
    jmp print
    end:
    
    ;new line
    mov al, 0xa
    int 0x10
    mov al, 0xd
    int 0x10
    
    popa
    ret
    