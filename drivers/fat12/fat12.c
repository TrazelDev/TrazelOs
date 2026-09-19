#include <drivers/ata_pio.h>
#include <drivers/block_device.h>
#include <include/ctype.h>
#include <include/mem_utils.h>
#include <include/types.h>
#include <kernel/include/heap.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "drivers/fat12.h"

static void* (*xmalloc)();
static void (*free)(void*);
static struct block_device* g_blk_dev;

void init_fat12(struct block_device* blk_dev) {
	xmalloc = kmalloc;
	free = kfree;
	g_blk_dev = blk_dev;
	printk("Initialized fat12 driver Successfully\n");
}

struct vfs_file* fat12_open(const char* filepath) {
	KERNEL_PANIC("fat12_open is not implemented yet");
	return NULL;
}
