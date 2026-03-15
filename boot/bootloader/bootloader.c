#include <drivers/ata_pio.h>
#include <drivers/block_device.h>
#include <drivers/vga_text.h>
#include <integer_utils.h>
#include <strings.h>
#include <types.h>

#include "bootloader_alloc.h"
#include "fat12.h"
#include "partition_table.h"

struct block_device get_bootable_partition_blk_device();
struct basic_allocator get_blk_dev_allocator();

void bootloader_entry() {
	set_cursor_position(position_to_coordinates(0, 0));
	print_string("Bootloader stage 2\n");

	struct block_device prt_blk_dev = get_bootable_partition_blk_device();
	struct basic_allocator dev_alloc = get_blk_dev_allocator();

	// FAT12Header fat12_header;
	// FAT12Info fat12_info;
	// init_fat12(&prt_blk_dev, &dev_alloc);
	// load_fat12_header(&fat12_header);
	// load_fat12_info(&fat12_info, &fat12_header);

	dev_alloc.free_allocator();
	asm volatile("hlt");
}

struct block_device get_bootable_partition_blk_device() {
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

	create_partition_wrapper(&prt_blk_device, blk_device, blk_lba_start, "t", blk_partition_size);
	return prt_blk_device;
}

struct basic_allocator get_blk_dev_allocator() {
	struct basic_allocator allocator = get_bootloader_basic_allocator();
	allocator.init((void*)0x100000, (void*)0x200000);

	return allocator;
}
