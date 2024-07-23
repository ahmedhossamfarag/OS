extern fopen_handler
extern fsize_handler
extern fclose_handler
extern fread_handler
extern fwrite_handler

extern prints_handler
extern scans_handler
extern scans_line_handler

syscall_map:
    dd 0
    dd fopen_handler ;0x1
    dd fsize_handler ;0x2
    dd fclose_handler ;0x3
    dd fread_handler ;0x4
    dd fwrite_handler ;0x5
    dd 0 ;0x6
    dd 0 ;0x7
    dd 0 ;0x8
    dd 0 ;0x9
    dd 0 ;0xA
    dd 0 ;0xB
    dd 0 ;0xC
    dd 0 ;0xD
    dd 0 ;0xE
    dd 0 ;0xF
    dd prints_handler ;0x10
    dd scans_handler ;0x11
    dd scans_line_handler ;0x12
    dd 0

syscall_entry:
    push eax
    mov eax, 0x10
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    pop eax

    shl esi, 2
    ; call [syscall_map+esi]

    push eax
    mov eax, 0x23
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    pop eax
    
    iret



%macro pushm 1
    mov edx, %1
    push edx 
%endmacro

global isr_syscall_handler
isr_syscall_handler:
    push edx
    push eax
    mov eax, esp
    mov esp, [eax+20]
    pushm [eax+24]
    pushm [eax+20]
    pushm [eax+16]
    pushm [eax+12]
    pushm [eax+8]
    pushm [eax+16]
    push cs
    push syscall_entry
    mov edx, [eax+4]
    mov eax, [eax]
    iret