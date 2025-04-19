#include <types.h>
#include "print.h"

#define PRINT_BUFFER_MAX 40
static uint64_t global_cursor_position = 0;

void set_cursor_position(uint16_t position)
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
    global_cursor_position = position;
}


uint16_t position_to_cordinates(uint8_t x, uint8_t y) {
    return (y * VGA_WIDTH) + x;
}




void print_string(const char* str) {
    uint16_t index = global_cursor_position;
    int strIndex = 0;

    while(str[strIndex] != '\0')
    {
        // using a switch to check if it is a normal char or there is a special char that needs to be printed:
        switch (str[strIndex])
        {
            case NEW_LINE:
                index += VGA_WIDTH;
                index -= (index % VGA_WIDTH + 1);
                break;
            case BACK_SPACE:
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


void print_char(char ch) {
    static char str[2] = { NULL, NULL };
    str[0] = ch;
    print_string(str);
}
