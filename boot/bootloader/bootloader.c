#include <drivers/vga_text.h>
#include <drivers/ata_pio.h>
#include <types.h>


void bootloader_entry() {
	set_cursor_position(position_to_cordinates(0, 0));
	print_string("Bootloader stage 2\n");

	ata_pio_init();
	// WARNING: This test breaks the bootsector, so run the os and than rebuild it
	// after so it will run
	if (ata_pio_min_test()) print_string("Test successful");
	
	asm volatile("hlt");
}
