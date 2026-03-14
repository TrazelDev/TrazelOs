#include <drivers/ata_pio.h>
#include <drivers/block_device.h>
#include <drivers/vga_text.h>
#include <integer_utils.h>
#include <strings.h>
#include <types.h>

#include "bootloader_alloc.h"
#include "partition_table.h"

void bootloader_entry() {
	set_cursor_position(position_to_coordinates(0, 0));
	print_string("Bootloader stage 2\n");

	struct block_device* blk_device = ata_pio_init();

	struct mbr_partition_table mbr_table = get_drive_mbr_partition_table(blk_device);
	print_mbr_partition_table(mbr_table);

	struct block_device prt_blk_device;
	size_t blk_lba_start;
	size_t blk_partition_size;
	for (int i = 0; i < MBR_MAX_PARTITION_TABLE_COUNT; i++) {
		if (is_partition_bootable(&mbr_table.partitions[i])) {
			blk_lba_start = mbr_table.partitions[i].relative_sectors_lba_start;
			blk_partition_size = mbr_table.partitions[i].total_sectors;
			break;
		}
	}
	create_partition_wrapper(&prt_blk_device, blk_device, blk_lba_start, blk_partition_size, "");
	asm volatile("hlt");
}

/* void small_test_alloc() {
	struct basic_allocator allocator = get_bootloader_basic_allocator();
	allocator.init((void*)0x20000, (void*)0x20500);
	char* addr = allocator.malloc(0x100);
	addr = allocator.malloc(200);
	itoa_unsigned((uint64_t)addr, addr, HEX);
	print_string("allocated addr: 0x");
	print_string(addr);
	print_string("\n");
	allocator.free_allocator();
} */
