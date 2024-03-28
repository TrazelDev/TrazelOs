OUTPUT_FORMAT(binary)

ENTRY (main)

SECTIONS
{
    . = 0x0008;

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
    
    . = ALIGN(0x1000);
    stack_region : {
        . += 5 * 0x1000;
    }
    _end = .;

    . = 0x0;
    .process_size : {
        QUAD(_end) 
    }
}