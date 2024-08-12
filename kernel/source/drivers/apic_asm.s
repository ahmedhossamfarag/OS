.global disable_pic
disable_pic:
    mov $0xFF, %al
    out %al, $0x21
    out %al, $0xA1
    ret

.global detect_apic
detect_apic:
    mov $1, %eax
    cpuid
    mov %edx, %eax
    and $0x200, %eax
    ret

.global enable_lapic
enable_lapic:
    mov $0x1B, %eax
    rdmsr
    bts $11, %eax # Set the APIC enable bit
    wrmsr
    ret
