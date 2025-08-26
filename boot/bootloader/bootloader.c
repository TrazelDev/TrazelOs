#include <drivers/vga_text.h>
#include <drivers/ata_pio.h>
#include <integer_utils.h>
#include <strings.h>
#include <types.h>
#include "partion_table.h"

void bootloader_entry() {
	set_cursor_position(position_to_cordinates(0, 0));
	print_string("Bootloader stage 2\n");
	
	ata_pio_init();
	
	// Getting a partion table:
	struct mbr_partion_table mbr_table = get_drive_mbr_partion_table();
	print_mbr_partion_table(mbr_table);

	asm volatile("hlt");
}
