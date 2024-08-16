NameLn equ 100
FILE_TYPE equ 0x33
DIR_TYPE equ 0x55

RLBA equ 0x20000
DLBA equ 0x20002

%macro fsentity 7 ; type, lba, parent-lba, prev-lba, next-lba, name, name-ln
    %%start:
        db %1 ; type
        dd %2 ; lba
        dd %3 ; parent
        dd %4 ; prev
        dd %5 ; next
    %%name:
        db %6 ; name
        times (NameLn - %7) db 0 ; name len
%endmacro

%macro dir 9 ; lba, parent-lba, prev-lba, next-lba, name, name-ln, head-lba, tail,-lba n-childs
    fsentity DIR_TYPE, %1, %2, %3, %4, %5, %6
    dd %7 ; head
    dd %8 ; tail
    dd %9 ; n-childs
    times 512 - 129 db 0
%endmacro

%macro file 9 ; lba, parent-lba, prev-lba, next-lba, name, name-ln, data-lba, n-blocks, size
    fsentity FILE_TYPE, %1, %2, %3, %4, %5, %6
    dd %7 ; data
    dd %8 ; n-blocks
    dd %9 ; size
    times 512 - 129 db 0
%endmacro

rootdir:
    dir RLBA, 0, 0, 0, 0, 1, RLBA + 1, RLBA + 1, 1

%assign rlba (RLBA+1)
%assign dlba (DLBA)

shell:
    file rlba, RLBA, 0, 0, "shell", 5, dlba, 50, 50*512

%assign rlba rlba + 1
%assign dlba dlba + 50