.global disable_pic
disable_pic:
    mov $0xFF, %al
    out %al, $0x21
    out %al, $0xA1
    ret

.global detect_apic
detect_apic:
    mov $1, %rax
    cpuid
    mov %rdx, %rax
    and $0x200, %rax
    ret

.global enable_lapic
enable_lapic:
    mov $0x1B, %rcx
    rdmsr
    bts $11, %rax # Set the APIC enable bit
    wrmsr
    ret
