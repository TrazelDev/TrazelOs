#include <include/mem_utils.h>

#include "block_device.h"

void create_partition_wrapper(struct block_device* new_part, struct block_device* parent_disk,
							  size_t start_lba, const char* part_name, size_t partition_size) {
	*new_part = *parent_disk;

	new_part->blocks_count = partition_size;
	new_part->partition_offset = parent_disk->partition_offset + start_lba;

	size_t i = 0;
	while (part_name[i] != '\0' && i < BLOCK_DEVICE_NAME_LEN - 1) {
		new_part->name[i] = part_name[i];
		i++;
	}
	new_part->name[i] = '\0';
}

#define MAX_BLOCK_SIZE 512
size_t read_blk_device_bytes(struct block_device* dev, size_t byte_offset, void* buf, size_t size) {
	if (dev->block_size > MAX_BLOCK_SIZE) {
		return 0;  // ERROR
	}

	uint8_t read_buffer[MAX_BLOCK_SIZE];
	uint8_t* dstbuf = (uint8_t*)buf;
	size_t remaining_bytes_to_read = size;
	size_t curr_lba = byte_offset / dev->block_size;
	size_t head_offset = byte_offset % dev->block_size;

	/* Reading the head of head (unalinged initial bytes) */
	if (head_offset > 0) {
		size_t head_bytes_to_copy = dev->block_size - head_offset;
		if (head_bytes_to_copy > remaining_bytes_to_read) {
			head_bytes_to_copy = remaining_bytes_to_read;
		}

		dev->read_block(dev, curr_lba, read_buffer);
		memcpy(dstbuf, read_buffer + head_offset, head_bytes_to_copy);
		dstbuf += head_bytes_to_copy;
		curr_lba++;
		remaining_bytes_to_read -= head_bytes_to_copy;
	}

	/* Reading the body (the aligned middle part) */
	while (remaining_bytes_to_read >= dev->block_size) {
		dev->read_block(dev, curr_lba, dstbuf);

		dstbuf += dev->block_size;
		remaining_bytes_to_read -= dev->block_size;
		curr_lba++;
	}

	if (remaining_bytes_to_read == 0) {
		return size;
	}

	/* Reading the tail (the unaligned last bytes) */
	dev->read_block(dev, curr_lba, read_buffer);
	memcpy(dstbuf, read_buffer, remaining_bytes_to_read);
	return size;
}
#undef MAX_BLOCK_SIZE
