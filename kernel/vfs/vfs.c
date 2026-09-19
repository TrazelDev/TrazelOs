#include <drivers/ata_pio.h>
#include <drivers/fat12.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "kernel/include/vfs.h"

void vfs_init() {
	struct block_device* blk_dev = ata_pio_init();
	init_fat12(blk_dev);
	printk("initialized VFS successfully.\n");
}

struct vfs_file* vfs_open(const char* filename) { KERNEL_PANIC("vfs_open is not implemented yet"); }
