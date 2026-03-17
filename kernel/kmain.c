// NOTE: This is not the real kernel but one for testing purposes in the refactoring of the
// bootloader The real kernel is src/kernel
#include <drivers/vga_text.h>

int kmain() {
	set_cursor_position(position_to_coordinates(0, 0));
	print_string("Hello World, from the kernel!");
	asm volatile("hlt");
	return 0;
}
