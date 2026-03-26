OUTPUT_FORMAT(elf64-x86-64)
ENTRY(kmain)

SECTIONS {
  . = 0xFFFFFFFFC0100000;

  .text : {
    *(.text)
    *(.text.*)
  }


  .bss : {
    *(.bss)
    *(.bss.*)
  }


  .data : {
    *(.data)
    *(.data.*)
  }


  .rodata : {
    *(.rodata)
    *(.rodata.*)
  }





  /* --- DWARF Debugging Sections --- */
  /* Setting the address to '0' ensures they are not allocated in RAM */
  
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line .debug_line.* .debug_line_str) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_ranges   0 : { *(.debug_ranges) }
  
  /* Discard anything else we don't explicitly want */
  /DISCARD/ : {
    *(.comment)
    *(.note.GNU-stack)
  }
}
