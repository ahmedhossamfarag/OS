global disable_pic
disable_pic:
    mov al, 0xFF
    out 0x21, al
    out 0xA1, al
    ret

global detect_apic
detect_apic:
    mov eax, 1
    cpuid
    mov eax, edx
    and eax, 0x200
    ret

global enable_lapic
enable_lapic:
    mov eax, 0x1B
    rdmsr
    bts eax, 11 ; Set the APIC enable bit
    wrmsr
    ret
