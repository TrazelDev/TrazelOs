#include <drivers/ata_pio.h>
#include <drivers/vga_text.h>
#include <integer_utils.h>
#include <strings.h>
#include <types.h>

#include "partition_table.h"

void bootloader_entry() {
	set_cursor_position(position_to_coordinates(0, 0));
	print_string("Bootloader stage 2\n");

	ata_pio_init();

	// Getting a partition table:
	struct mbr_partition_table mbr_table = get_drive_mbr_partition_table();
	print_mbr_partition_table(mbr_table);

	asm volatile("hlt");
}
