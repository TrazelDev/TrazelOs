OUTPUT_FORMAT(binary)
ENTRY (main)

SECTIONS
{
    . = 0x0;
    .startingAddress :
    {
        QUAD(_next_process);
        QUAD(0);
    }

    . = 0x8;
    .main_location :
    {
        *(.main_location)
    }

    .text : ALIGN(0x500)
    {
        . = . + SIZEOF_HEADERS;
        *(.text)
    }

    .data : ALIGN(0x200)
    {
        *(.data)
    }

    .bss : ALIGN(0x200)
    {
        *(COMMON)
        *(.bss)
    }


    .bss_end : ALIGN(0x1000)
    {
        BYTE(0)
        . = (. + 0x1000 - 1);
    }
    _next_process = .;
}
