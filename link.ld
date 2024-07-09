 OUTPUT_FORMAT(elf32-i386)

 SECTIONS {
     . = 0x1000; /* Set the base address for the executable */
     .text : { *(.text) *(.text.*) }
     .rodata : { *(.rodata) *(.rodata.*) }
     .data : { *(.data) *(.data.*) }
     .bss : { *(.bss) *(.bss.*) }
     .got : { *(.got) *(.got.*) }
 }