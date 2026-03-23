OUTPUT_FORMAT(elf64-x86-64)
ENTRY(kmain)

PHDRS {
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2)) ; /* execute + read */
    rodata  PT_LOAD    FLAGS((1 << 2)) ;            /* read */
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ; /* write + read */
}

SECTIONS {
    . = 0xFFFFFFFFC0100000;

    .text ALIGN(CONSTANT(MAXPAGESIZE)) : {
        *(.text .text.*)
    } : text

    .rodata ALIGN(CONSTANT(MAXPAGESIZE)) : {
        *(.rodata .rodata.*)
    } : rodata

    .data ALIGN(CONSTANT(MAXPAGESIZE)) : {
        *(.data .data.*)
    } : data

    .bss : {
        *(.COMMON)
        *(.bss .bss.*)
    } : data

    /* --- DWARF Debugging Sections --- */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line .debug_line.* .debug_line_str) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_ranges   0 : { *(.debug_ranges) }
 
    /DISCARD/ : {
        *(.comment)
        *(.note.GNU-stack)
    }
}
