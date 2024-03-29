OUTPUT_FORMAT(binary)

ENTRY (main)

SECTIONS
{
    . = 0x0;
    .main_location : {
        *(.main_location)
    }

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
    
    stack_start = .;
    stack_region : {
        . += 5 * 0x1000;
    }
    
    .bss_end : ALIGN(01000)
    {
        BYTE(0)
    }
    _end = . - 1;
}