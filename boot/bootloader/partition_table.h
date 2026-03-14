#pragma once
#include <types.h>

#define MBR_MAX_PARTITION_TABLE_COUNT 4

struct chs_address {
	uint8_t head;
	uint16_t sector : 6;
	uint16_t cylinder : 10;
} __attribute__((packed));

struct mbr_partition_table_entry {
	uint8_t drive_attributes;
	struct chs_address start_addr;
	uint8_t system_id;
	struct chs_address end_addr;
	uint32_t relative_sectors_lba_start;
	uint32_t total_sectors;
} __attribute__((packed));

struct mbr_partition_table {
	struct mbr_partition_table_entry partitions[MBR_MAX_PARTITION_TABLE_COUNT];
} __attribute__((packed));

struct mbr_partition_table get_drive_mbr_partition_table(struct block_device* blk_device);
void print_mbr_partition_table(struct mbr_partition_table mbr_table);
static inline bool is_partition_bootable(struct mbr_partition_table_entry* entry) {
	return entry->drive_attributes & 0x80;
}
