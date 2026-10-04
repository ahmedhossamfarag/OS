; Switch to long mode and enable paging. This code is executed in 32-bit protected mode, and it sets up the necessary structures for long mode and paging.


%include "define.asm"

[org KERNEL_OFFSET]

%include "long_mode_paging.asm"

[bits 32]
; Create a paging structure for the kernel to use. This is a simple identity mapping of the first 4GB of memory, using PML4.

; Load PML4
mov eax, FIRST_DIR_ALIGN
mov cr3, eax

; Enable PAE, OSFXSR, and OSXSAVE
mov eax, cr4
or  eax, 1 << 5          ; CR4.PAE
or  eax, 1 << 9          ; CR4.OSFXSR
or  eax, 1 << 10         ; CR4.OSXSAVE
mov cr4, eax

; Enable Long Mode Enable (LME)
mov ecx, 0xC0000080      ; IA32_EFER
rdmsr
or  eax, 1 << 8          ; EFER.LME
wrmsr

; Enable paging
mov eax, cr0
or  eax, 1 << 31         ; CR0.PG
mov cr0, eax

; Enter 64-bit code segment
jmp 0x18: long_mode_entry ; Make a far jump


; -----------------------------
; 64-bit long mode
; -----------------------------

[bits 64]

long_mode_entry:
mov ax, 0x20
mov ds, ax
mov es, ax
mov ss, ax
mov fs, ax
mov gs, ax
xor rax, rax
cli

times 0x100 -( $ - $$ ) nop
; Now we are in long mode, and we can continue executing 64-bit code.
