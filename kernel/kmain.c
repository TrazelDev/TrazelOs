// NOTE: This is not the real kernel but one for testing purposes in the refactoring of the
// bootloader The real kernel is src/kernel
#include <drivers/vga_text.h>

int kmain() {
	asm volatile("hlt");
	return 0;
}
