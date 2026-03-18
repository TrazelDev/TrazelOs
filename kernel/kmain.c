// NOTE: This is not the real kernel but one for testing purposes in the refactoring of the
// bootloader The real kernel is src/kernel
#include <drivers/vga_text.h>

#include "kernel/printk/printk.h"

int kmain() {
	init_printk();
	printk("Hello world, from the kernel\n");

	while (true) {
		asm volatile("hlt");
	}
}
