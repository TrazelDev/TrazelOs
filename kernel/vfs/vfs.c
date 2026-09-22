#include <drivers/ata_pio.h>
#include <drivers/block_device.h>
#include <drivers/fat12.h>
#include <include/mem_utils.h>
#include <kernel/include/heap.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "kernel/include/vfs.h"

#define FIRST_PARTITION_MBR_OFFSET 2048
void vfs_init() {
	struct block_device* prt_blk_dev = ata_pio_init();

	create_partition_wrapper(prt_blk_dev, ata_pio_init(), FIRST_PARTITION_MBR_OFFSET, "/dev/sda1",
							 -1);
	init_fat12(prt_blk_dev);
	printk("initialized VFS successfully.\n");
}

struct vfs_file* vfs_open(const char* filename) { return fat12_open(filename); }
