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

struct vfs_file* vfs_open(const char* filename) {
	struct vfs_file* file_vfs = fat12_open(filename);
	file_vfs->file_position = 0;
	file_vfs->file_ref_count = 1;

	return file_vfs;
}

int64_t vfs_read(struct vfs_file* vfs_node, uint8_t* buffer, uint64_t size) {
	if (!vfs_node || !vfs_node->read) {
		return -1;
	}

	if (vfs_node->file_position >= vfs_node->file_size) {
		return 0;  // EOF
	}

	uint64_t bytes_to_read = size;
	if (vfs_node->file_position + bytes_to_read > vfs_node->file_size) {
		bytes_to_read = vfs_node->file_size - vfs_node->file_position;
	}

	int64_t bytes_read = vfs_node->read(vfs_node, buffer, bytes_to_read);
	if (bytes_read > 0) {
		vfs_node->file_position += bytes_read;
	}

	return bytes_read;
}

int64_t vfs_close(struct vfs_file* vfs_node) {
	if (!vfs_node) {
		return -1;
	}

	if (vfs_node->file_ref_count > 0) {
		vfs_node->file_ref_count--;
	}

	if (vfs_node->file_ref_count == 0) {
		vfs_node->close(vfs_node);
		kfree(vfs_node);
	}

	return 0;
}
int64_t vfs_seek(struct vfs_file* vfs_node, int64_t offset, enum vfs_seek_whence whence) {
	if (!vfs_node) {
		return -1;
	}

	int64_t new_pos = 0;
	switch (whence) {
		case SKW_VFS_SEEK_SET:
			new_pos = offset;
			break;
		case SKW_VFS_SEEK_CUR:
			new_pos = (int64_t)vfs_node->file_position + offset;
			break;
		case SKW_VFS_SEEK_END:
			new_pos = (int64_t)vfs_node->file_size + offset;
			break;
		default:
			return -1;
	}

	if (new_pos < 0 || new_pos > (int64_t)vfs_node->file_size) {
		return -1;
	}

	vfs_node->file_position = new_pos;
	return new_pos;
}
