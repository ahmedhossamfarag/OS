 ENTRY(_start)

 SECTIONS {
     . = 0xA000000; /* Set the base address for the executable */
	
    .text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
		_ap_setup_start = .;
		KEEP(*(.ap_setup))
		_ap_setup_end = .;
		_font_bitmap = .;
		KEEP(*(.font_bitmap))
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

 }