#pragma once

enum class InterruptIndexes
{
    // software interrupts:
    double_fault = 0x8,
    page_fault   = 0xE,


    // hardware interrupts:
    pit      = 0x20,
    keyboard = 0x21,
    cmos_rtc = 0x27,
};