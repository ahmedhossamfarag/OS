global jump_usermode
extern usermode_entry
jump_usermode:
	mov ax, 0x20 | 3 ; ring 3 data with bottom 2 bits set for ring 3
	mov ds, ax
	mov es, ax 
	mov fs, ax 
	mov gs, ax ; SS is handled by iret

    mov esp, 0x80000

	push 0x20 | 3 ; data selector
	push 0x80000 ; current esp
	pushf ; eflags
	push 0x18 | 3 ; code selector (ring 3 code with bottom 2 bits set for ring 3)
	push usermode_entry ; instruction address to return to
	iret