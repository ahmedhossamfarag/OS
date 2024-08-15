times 446 - ($-$$) db 0

boot_partition:
    db 1 ; bootable
    db 0 ; start head
    dw 0 ; start cylinder & sector
    db 0 ; partition type
    db 0 ; end head
    dw 0 ; end cylinder & sector
    dd 1 ; start lba
    dd 200 ; end lba

data_partition:
    db 0 ; bootable
    db 0 ; start head
    dw 0 ; start cylinder & sector
    db 0 ; partition type
    db 0 ; end head
    dw 0 ; end cylinder & sector
    dd 201 ; start lba
    dd 0 ; end lba

unused_partition_1:
    db 0 ; bootable
    db 0 ; start head
    dw 0 ; start cylinder & sector
    db 0 ; partition type
    db 0 ; end head
    dw 0 ; end cylinder & sector
    dd 0 ; start lba
    dd 0 ; end lba

unused_partition_2:
    db 0 ; bootable
    db 0 ; start head
    dw 0 ; start cylinder & sector
    db 0 ; partition type
    db 0 ; end head
    dw 0 ; end cylinder & sector
    dd 0 ; start lba
    dd 0 ; end lba
