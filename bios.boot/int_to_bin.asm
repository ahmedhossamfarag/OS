; convert int @ax to bin string $bx
int_to_bin:
    pusha
    mov cx , 16
    convert:
        mov dx , ax
        and dx , 1
        add dx , '0'
        mov [bx] , dl
        inc bx
        shr ax , 1
        loop convert
        mov dl , 0
        mov [bx] , dl
    popa
    ret