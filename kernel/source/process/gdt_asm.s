.global enable_gdt_asm
enable_gdt_asm:
    pushq $0x10
    leaq reload_segments(%rip), %rax
    push %rax
    leaq gdtp(%rip), %rax
    lgdt (%rax)
    lretq
    hlt

reload_segments:
    mov $0x20, %rax
    mov %rax, %ds
    mov %rax, %es
    mov %rax, %fs
    mov %rax, %ss
    mov %rax, %gs
    nop
    ret