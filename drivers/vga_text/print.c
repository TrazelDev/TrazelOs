#include <types.h>
#include <io.h>
#include "drivers/vga_text.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY (uint8_t*)0xB8000
#define VGA_LAST_MEMORY_ADDRESS (0xB8000 + (VGA_WIDTH * VGA_HEIGHT * 2))

#define POSITION_LOWER_BYTES(position) (uint8_t)( position		  & 0xFF)
#define POSITION_UPPER_BYTES(position) (uint8_t)((position >> 8 ) & 0xFF)

#define PRINT_BUFFER_MAX 40
static uint64_t global_cursor_position = 0;

void set_cursor_position(uint16_t position) {
	// tell the VGA controller that we're setting the low byte of the cursor position
	outb(IO_vga_register_port, 0x0F);

	// set the low byte of the cursor position
	outb(IO_vga_data_port, POSITION_LOWER_BYTES(position));

	// tell the VGA controller that we're setting the high byte of the cursor position
	outb(IO_vga_register_port, 0x0E);

	// set the high byte of the cursor position
	outb(IO_vga_data_port, POSITION_UPPER_BYTES(position));
 
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
				// *(VGA_MEMORY + (index * 2)) = ' ';
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

	set_cursor_position(index);
}


void print_char(char ch) {
	static char str[2] = { NULL, NULL };
	str[0] = ch;
	print_string(str);
}
