#include <drivers/vga_text.h>

void bootloader_entry() {
	set_cursor_position(position_to_cordinates(0, 0));
	print_string("Hello world from bootloader\n");
	asm volatile("hlt");
}
