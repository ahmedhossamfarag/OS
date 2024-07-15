[ bits 16]

cli ; switch off interrupts

lgdt [ gdt_descriptor ] ; Load our global descriptor table , which defines the protected mode segments ( e.g. for code and data )

mov eax , cr0 ; To make the switch to protected mode , we set cr0 first bit to 1
or eax , 0x1
mov cr0 , eax

jmp CODE_SEG : init_pm ; Make a far jump 

[ bits 32]
; Initialise registers and the stack once in PM.
init_pm :
mov ax , DATA_SEG ;
mov ds , ax
mov ss , ax
mov es , ax
mov fs , ax
mov gs , ax

mov ebp , PROTECTED_MODE_SP ; Update our stack position so it is right
mov esp , ebp

; Shift Kernel
mov esi , KERNEL_LOAD_OFFSET
mov edi , KERNEL_OFFSET
mov ecx , KERNEL_N_SECTORS * 512
kernel_shift:
    mov eax , [esi]
    mov [edi] , eax
    add esi , 4
    add edi , 4
    loop kernel_shift

call KERNEL_OFFSET ; Finally , call the kernel
jmp $
