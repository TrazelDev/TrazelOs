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

static inline void create_partition_wrapper(struct block_device* new_part,
											struct block_device* parent_disk, size_t start_lba,
											size_t partition_size, const char* part_name) {
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
