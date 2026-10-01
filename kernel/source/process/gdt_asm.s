.global enable_gdt_asm
enable_gdt_asm:
    lgdt gdtp
    pushq $0x08
    leaq 1f(%rip), %rax
    pushq %rax
    lretq

1:
    mov $0x10, %eax
    mov %eax, %ds
    mov %eax, %es
    mov %eax, %fs
    mov %eax, %ss
    mov %eax, %gs
    nop
    ret