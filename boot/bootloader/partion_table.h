#pragma once
#include <types.h>

#define MBR_MAX_PARTION_TABLE_COUNT 4

struct chs_address {
	uint8_t head;
	uint16_t sector : 6;
	uint16_t cylinder : 10;
} __attribute__((packed));

struct mbr_partion_table_entry {
	uint8_t drive_attributes;
	struct chs_address start_addr;
	uint8_t system_id;
	struct chs_address end_addr;
	uint32_t relative_sectors_lba_start;
	uint32_t total_sectors;
} __attribute__((packed));

struct mbr_partion_table {
	struct mbr_partion_table_entry partions[MBR_MAX_PARTION_TABLE_COUNT];
} __attribute__((packed));

struct mbr_partion_table get_drive_mbr_partion_table();
void print_mbr_partion_table(struct mbr_partion_table mbr_table);
