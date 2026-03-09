#include <drivers/ata_pio.h>
#include <drivers/vga_text.h>
#include <integer_utils.h>
#include <strings.h>
#include <types.h>

#include "bootloader_alloc.h"
#include "partition_table.h"

void bootloader_entry() {
	set_cursor_position(position_to_coordinates(0, 0));
	print_string("Bootloader stage 2\n");

	ata_pio_init();

	struct mbr_partition_table mbr_table = get_drive_mbr_partition_table();
	print_mbr_partition_table(mbr_table);

	struct basic_allocator allocator = get_bootloader_basic_allocator();
	allocator.init((void*)0x20000, (void*)0x20500);
	char* addr = allocator.malloc(0x100);
	addr = allocator.malloc(200);
	itoa_unsigned(addr, addr, HEX);
	print_string("allocated addr: 0x");
	print_string(addr);
	print_string("\n");
	allocator.free_allocator();

	asm volatile("hlt");
}
