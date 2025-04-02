OUTPUT_FORMAT(binary)

SECTIONS {
	. = 0x08000;
	.text : { 
		*(.text) 
	}
	.data : { *(.data) }
	.bss : {
		*(.COMMON)
		*(.bss)
	}
}
