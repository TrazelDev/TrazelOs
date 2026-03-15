#pragma once
#include <include/types.h>

#define BLOCK_DEVICE_NAME_LEN 100

struct block_device {
	char name[BLOCK_DEVICE_NAME_LEN];
	size_t block_size;
	size_t blocks_count;
	size_t partition_offset;

	ssize_t (*read_block)(struct block_device* blk_device, size_t lba, uint8_t* buffer);
	ssize_t (*write_block)(struct block_device* blk_device, size_t lba, uint8_t* buffer);
};

void create_partition_wrapper(struct block_device* new_part, struct block_device* parent_disk,
							  size_t start_lba, const char* part_name, size_t partition_size);

/** Takes a block device, a buffer and size and just reads it arbitrary without regard to block
 * sizes.
 * @param dev The block device that read is requested from.
 * @param byte_offset The byte offset from the start of the device (not lba offset).
 * @param buf The destination memory that will be written to (need to be size bytes long at least).
 * @param size The size of the buffer and the amount of bytes we want to read.
 * @return Size for successful read, 0 for error.
 */
size_t read_blk_device_bytes(struct block_device* dev, size_t byte_offset, void* buf, size_t size);
