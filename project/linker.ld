OUTPUT_FORMAT(binary)

ENTRY (_start)

SECTIONS
{
    _KernelStart = .; /* the starting address of the kernel */
    . = 0x9000;

    .text : ALIGN(0x1000)
    {
        . = . + SIZEOF_HEADERS;
        *(.text)
    }

    .data : ALIGN(0x1000)
    {
        *(.data)
    }

    .bss : ALIGN(0x1000)
    {
        *(COMMON)
        *(.bss)
    }

    
    _KernelEnd = .; /* the last address of the kernel */
}