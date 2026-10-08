.set MEMORY_VIRTUAL_START, 0xFFFFFFFC00000000
.set KERNEL_END, 0x10000000
.set KERNEL_PRIVILEGE, 3
.set ENTRIES_PER_TABLE, 512
.set PAGE_SIZE, 4096 
.set PAGE_PS, 0x80

.section .text
.global default_virtual_memory_mapping
.type  default_virtual_memory_mapping, @function

default_virtual_memory_mapping:

    # rdi = pml4
    # r8  = pdpt
    # r9  = align / current PD
    # rcx = pdpt_index
    # rdx = pd_index
    # rax = physical memory address

    mov     $KERNEL_END, %r9          # align
    mov     $((MEMORY_VIRTUAL_START >> 39) & 0x1ff), %rcx
    mov     %r9, %r8                  # pdpt = align

    # pml4[pml4_index] = pdpt | flags
    mov     %r8, %rax
    or      $KERNEL_PRIVILEGE, %rax
    mov     %rax, (%rdi,%rcx,8)

    add     $PAGE_SIZE, %r9           # align = pdpt + PAGE_SIZE

    xor     %rax, %rax                # physical memory start = 0

    mov     $((MEMORY_VIRTUAL_START >> 30) & 0x1ff), %rcx

.Lpdpt:
    # pd = align
    # pdpt[pdpt_index] = pd | flags

    mov     %r9, %rbx
    or      $KERNEL_PRIVILEGE, %rbx
    mov     %rbx, (%r8,%rcx,8)

    # Now allocate the next page for the next PD
    add     $PAGE_SIZE, %r9

    xor     %rdx, %rdx                # pd_index = 0

.Lpd:
    # pd[rdx] = physical | flags
    mov     %rax, %rbx
    or      $KERNEL_PRIVILEGE | PAGE_PS, %rbx
    mov     %rbx, -PAGE_SIZE(%r9,%rdx,8)

    add     $0x200000, %rax       # next 2 MiB physical page
    inc     %rdx

    cmp     $ENTRIES_PER_TABLE, %rdx
    jb      .Lpd

    inc     %rcx
    cmp     $ENTRIES_PER_TABLE, %rcx
    jb      .Lpdpt

    ret


.global virtual_kernel
.type virtual_kernel, @function
virtual_kernel:
		pop %rax
        movabs $MEMORY_VIRTUAL_START, %rcx
        add %rcx, %rax
        add %rcx, %rsp
        push %rax
        ret
