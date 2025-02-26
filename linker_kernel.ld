OUTPUT_FORMAT(binary)
ENTRY(_start)

SECTIONS
{
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
    
    . = ALIGN(0x1000);
    _contextSwitch_addr = .;
    .contextSwitch_location : ALIGN(0x1000)
    {
        *(.contextSwitch_location)
        *(.globalPCB_location)
        *(.kernelPCB_location)
        *(.plm4TablePtr_location)
    }


    . = ALIGN(0x1000);
    .bss_end : ALIGN(0x1000)
    {
        BYTE(0)
        . = (. + 0x1000 - 1);
    }
    _next_process = .;
}
