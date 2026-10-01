.set FLAGS,    0x1FFF  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */


.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.long 0		#graphics mode type
.long 800	#width
.long 600	#height
.long 8		#depth

.section .text
.global _start
.type _start, @function
_start:
	mov %rax, bl_magic
	mov %rbx, boot_info_ptr
	mov %rcx, memory_info_ptr
	mov %rdx, graphics_info_ptr
	mov $0xB000000, %rsp
	call kernel_main
	cli
1:	hlt
	jmp 1b

.size _start, . - _start

.section .data
.global bl_magic
.global boot_info_ptr
.global memory_info_ptr
.global graphics_info_ptr
bl_magic:
	.quad 0
boot_info_ptr:
	.quad 0
memory_info_ptr:
	.quad 0
graphics_info_ptr:
	.quad 0