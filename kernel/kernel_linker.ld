OUTPUT_FORMAT(elf64-x86-64)
ENTRY(kmain)

SECTIONS {
  . = 0x100000;

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
}
