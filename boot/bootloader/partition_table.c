#include <drivers/ata_pio.h>
#include <drivers/vga_text.h>
#include <integer_utils.h>

#include "partition_table.h"

struct mbr_partition_table get_drive_mbr_partition_table() {
	unsigned char mbr_sector_buf[512];
	ata_pio_read(0, 1, mbr_sector_buf);

	return *(struct mbr_partition_table*)(mbr_sector_buf + 0x1BE);
}

void print_mbr_partition_table(struct mbr_partition_table mbr_table) {
	char tempbuf[100];
	print_string("partition num | boot | start | end | sectors | ID\n");

	bool bootflag = false;
	uint64_t start_sector = 0;
	uint64_t total_sectors = 0;
	uint64_t end_sector = 0;
	uint64_t system_id = 0;
	for (uint8_t i = 0; i < MBR_MAX_PARTITION_TABLE_COUNT; i++) {
		if (mbr_table.partitions[i].system_id == 0x00) {
			continue;
		}

		bootflag = mbr_table.partitions[i].drive_attributes & 0x80;
		start_sector = mbr_table.partitions[i].relative_sectors_lba_start;
		total_sectors = mbr_table.partitions[i].total_sectors;
		end_sector = start_sector + total_sectors - 1;
		system_id = mbr_table.partitions[i].system_id;

		// partition num:
		print_string(itoa_unsigned(i, tempbuf, DECIMAL));
		print_string("           |");

		// boot flag:
		print_string(bootflag ? " *" : "  ");
		print_string("    | ");

		// start sector:
		print_string(itoa_unsigned(start_sector, tempbuf, DECIMAL));
		print_string("     | ");

		// end sector:
		print_string(itoa_unsigned(end_sector, tempbuf, DECIMAL));
		print_string("  | ");

		// total sectors:
		print_string(itoa_unsigned(total_sectors, tempbuf, DECIMAL));
		print_string("      | ");

		// system id:
		print_string(itoa_unsigned(system_id, tempbuf, HEX));
		print_char('\n');
	}
}
