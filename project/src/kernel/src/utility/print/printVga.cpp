#include "printVga.h"
#include "utility/debug/assert.h"
#include "utility/io/io.h"
#include "utility/types/integer.h"

uint16_t cursorPostion = 0;

void setCursorPosition(uint16_t position)
{
    // tell the VGA controller that we're setting the low byte of the cursor position
    outb(IO_PORTS::vga_register_port, 0x0F);

    // set the low byte of the cursor position
    outb(IO_PORTS::vga_data_port, POSITION_LOWER_BYTES(position));

    // tell the VGA controller that we're setting the high byte of the cursor position
    outb(IO_PORTS::vga_register_port, 0x0E);

    // set the high byte of the cursor position
    outb(IO_PORTS::vga_data_port, POSITION_UPPER_BYTES(position));
    
    // updating the position variable:
    cursorPostion = position;
}


uint16_t postionFromCords(uint8_t x, uint8_t y)
{
    // in debug making sure that the x and y coordinates are within bounds
    ASSERT((x >= 0 && y >= 0 && x < VGA_WIDTH && y < VGA_HEIGHT));

    return (y * VGA_WIDTH) + x;
}


void printString(const char* str)
{
    uint16_t index = cursorPostion;
    int strIndex = 0;

    while(str[strIndex] != '\0')
    {
        // using a switch to check if it is a normal char or there is a special char that needs to be printed:
        switch (str[strIndex])
        {
            case (char)SPECIAL_CHARS::NEW_LINE:
                index += VGA_WIDTH;
                index -= (index % VGA_WIDTH + 1);
                break;
            case (char)SPECIAL_CHARS::BACK_SPACE:
                index--;
                *(VGA_MEMORY + (index * 2)) = ' ';
                index--;
                break;
            default:
                *(VGA_MEMORY + (index * 2)) = str[strIndex];
                break;
        }
        
        // going to the next char
        index++;
        strIndex++;
    }

    // updating the cursor position:
    setCursorPosition(index);
}

void printChar(char ch)
{
    static char str[2] = { NULL, NULL };
    str[0] = ch;
    printString(str);
}

/// @brief templated function that prints some sort of integer
/// @tparam T some sort of an integer
/// @param value the value that needs to be printed
template< typename T>
void printInt(T value)
{
    printString(intToString(value));
}

void printInt(uint8_t  value)  { printInt<uint8_t  >(value); }
void printInt(uint16_t value)  { printInt<uint16_t >(value); }
void printInt(uint32_t value)  { printInt<uint32_t >(value); }
void printInt(uint64_t value)  { printInt<uint64_t >(value); }
void printInt(char      value) { printInt<char     >(value); }
void printInt(short     value) { printInt<short    >(value); }
void printInt(int       value) { printInt<int      >(value); }
void printInt(long long value) { printInt<long long>(value); }

/// @brief templated function that prints some sort of hex
/// @tparam T some sort of an hex number
/// @param value the value that needs to be printed
template< typename T>
void printHex(T value)
{
    printString(hexToString(value));
}

void printHex(uint8_t  value) { return printHex<uint8_t >(value); };
void printHex(uint16_t value) { return printHex<uint16_t>(value); };
void printHex(uint32_t value) { return printHex<uint32_t>(value); };
void printHex(uint64_t value) { return printHex<uint64_t>(value); };