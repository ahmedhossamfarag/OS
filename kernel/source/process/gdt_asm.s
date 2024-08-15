.global enable_gdt_asm
enable_gdt_asm:
    lgdt gdtp
    ljmp $0x8, $gdt_resume

gdt_resume:
    mov $0x10, %eax
    mov %eax, %ds
    mov %eax, %es
    mov %eax, %fs
    mov %eax, %ss
    mov %eax, %gs
    nop
    ret