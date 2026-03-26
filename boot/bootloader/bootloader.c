#include <drivers/ata_pio.h>
#include <drivers/block_device.h>
#include <include/integer_utils.h>
#include <include/strings.h>
#include <include/types.h>

#include "bootloader_alloc.h"
#include "bootloader_print.h"
#include "fat12.h"
#include "hhdm.h"
#include "kernel_elf_loader.h"
#include "partition_table.h"

struct block_device get_bootable_partition_blk_device();
struct basic_allocator get_blk_dev_allocator();

void bootloader_entry() {
	init_print();
	print_string("Bootloader stage 2\n");

	struct block_device prt_blk_dev = get_bootable_partition_blk_device();
	struct basic_allocator dev_alloc = get_blk_dev_allocator();

	FAT12Header fat12_header;
	FAT12Info fat12_info;
	init_fat12(&prt_blk_dev, &dev_alloc);
	load_fat12_header(&fat12_header);
	load_fat12_info(&fat12_info, &fat12_header);

	char** names;
	uint32_t name_count = get_root_file_names(&names, &fat12_info);
	for (size_t i = 0; i < name_count; i++) {
		print_string(names[i]);
		print_string("\n");
	}

	FAT12DirectoryEntry* dir_entries;
	char* file_content;
	get_root_directory_entries(&dir_entries, &fat12_info);

	init_kernel_hhdm(&dev_alloc);

	uint64_t file_size = get_file_content((uint8_t**)&file_content, dir_entries, &fat12_info);
	int kernel_return_value = load_and_jump_kernel(file_content, file_size);

	dev_alloc.free_allocator();
	while (true) {
		asm volatile("hlt");
	}
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
	allocator.init((void*)0x180000, (void*)0x200000);

	return allocator;
}
