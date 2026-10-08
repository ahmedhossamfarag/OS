KERNEL_VMA = 0xFFFFFFFC0A000000;
KERNEL_LMA = 0xA000000;
OFFSET     = KERNEL_VMA - KERNEL_LMA;

 ENTRY(_start)

 SECTIONS {
     . = KERNEL_VMA; /* Set the base address for the executable */

	 . = ALIGN(4K);
	
    .text BLOCK(4K) : AT(ADDR(.text) - OFFSET)
	{
		*(.text)
	}

	.rodata BLOCK(4K) : AT(ADDR(.rodata) - OFFSET)
	{
		*(.rodata)
		_ap_setup_start = .;
		KEEP(*(.ap_setup))
		_ap_setup_end = .;
		_font_bitmap = .;
		KEEP(*(.font_bitmap))
	}

	.data BLOCK(4K) : AT(ADDR(.data) - OFFSET)
	{
		*(.data)
	}

	.bss BLOCK(4K) : AT(ADDR(.bss) - OFFSET)
	{
		*(COMMON)
		*(.bss)
	}

 }