 ENTRY(_start)

 SECTIONS {
     . = 2M; /* Set the base address for the executable */
    
    .text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		 _kernel_start = .;
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

	 _kernel_end = .;
 }