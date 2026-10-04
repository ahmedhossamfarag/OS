; This file is part of the BIOS bootloader for an operating system. It sets up the paging structures required for long mode and enables paging. The code is written in x86 assembly language and is intended to be executed in 32-bit protected mode.

PAGE_4K          equ 4096
PAGE_SIZE        equ 4096
NUM_PDPT_ENTRIES equ 4          ; 4GB RAM / 1GB per PDPT entry = 4 entries
ENTRIES_PER_TABLE equ 512
KERNEL_PRIVILEGE equ 3           ; Present | RW
PAGE_PS          equ 0x80
FIRST_DIR_ALIGN  equ 0x7C0000    ; Address for the paging structures, aligned to 4KB


[bits 32]
; ============================================================

mov     esi, FIRST_DIR_ALIGN

; ============================================================
; pml4 = align
; align += 4096
; ============================================================

mov     ebx, esi                    ; ebx = pml4
add     esi, PAGE_4K

; ============================================================
; pdpt = align
; align += 4096
; ============================================================

mov     edi, esi                    ; edi = pdpt
add     esi, PAGE_4K

; ============================================================
; Zero PML4
; ============================================================

xor     eax, eax
mov     ecx, PAGE_SIZE / 4
mov     edx, ebx

.zero_pml4:
    mov     [edx], eax
    add     edx, 4
    loop    .zero_pml4

; ============================================================
; Zero PDPT
; ============================================================

mov     ecx, PAGE_SIZE / 4
mov     edx, edi

.zero_pdpt:
    mov     [edx], eax
    add     edx, 4
    loop    .zero_pdpt

; ============================================================
; pml4[0] = pdpt | KERNEL_PRIVILEGE
; ============================================================

mov     eax, edi
or      eax, KERNEL_PRIVILEGE

mov     [ebx], eax
mov     [ebx + 4], dword 0

; ============================================================
; pdpt_index = 0
; ============================================================

xor     ebp, ebp

.pdpt_loop:

    ; --------------------------------------------------------
    ; pd = align
    ; align += 4096
    ; --------------------------------------------------------

    mov     ebx, esi            ; ebx = pd
    add     esi, PAGE_4K

    ; --------------------------------------------------------
    ; Zero PD
    ; --------------------------------------------------------

    xor     eax, eax
    mov     ecx, PAGE_SIZE / 4
    mov     edx, ebx

.zero_pd:
    mov     [edx], eax
    add     edx, 4
    loop    .zero_pd

    ; --------------------------------------------------------
    ; pd_index = 0
    ; --------------------------------------------------------

    xor     ecx, ecx

.pd_loop:

        ; ====================================================
        ; physical_address =
        ;
        ;   (pdpt_index << 30) |
        ;   (pd_index   << 21)
        ;
        ; Construct the 64-bit value as EDX:EAX.
        ; ====================================================

        ; Low 32 bits = (pd_index << 21)
        ;              | low 2 bits of pdpt_index << 30

        mov     eax, ecx
        shl     eax, 21

        mov     edx, ebp
        and     edx, 3
        shl     edx, 30

        or      eax, edx

        ; High 32 bits = pdpt_index >> 2
        mov     edx, ebp
        shr     edx, 2

        ; ====================================================
        ; Add flags
        ; ====================================================

        or      eax, KERNEL_PRIVILEGE | PAGE_PS

        ; ====================================================
        ; pd[pd_index] = EDX:EAX
        ;
        ; offset = pd_index * 8
        ; ====================================================

        push    ebp

        mov     ebp, ecx
        shl     ebp, 3

        mov     [ebx + ebp], eax
        mov     [ebx + ebp + 4], edx

        pop     ebp

        ; ----------------------------------------------------
        ; pd_index++
        ; ----------------------------------------------------

        inc     ecx
        cmp     ecx, ENTRIES_PER_TABLE
        jb      .pd_loop

    ; ========================================================
    ; pdpt[pdpt_index] = pd | KERNEL_PRIVILEGE
    ; ========================================================

    mov     eax, ebx
    or      eax, KERNEL_PRIVILEGE

    mov     ebx, ebp
    shl     ebx, 3

    mov     [edi + ebx], eax
    mov     [edi + ebx + 4], dword 0

    ; ========================================================
    ; pdpt_index++
    ; ========================================================

    inc     ebp
    cmp     ebp, NUM_PDPT_ENTRIES
    jb      .pdpt_loop