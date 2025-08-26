#include "partion_table.h"
#include <drivers/ata_pio.h>
#include <drivers/vga_text.h>
#include <integer_utils.h>

struct mbr_partion_table get_drive_mbr_partion_table() {
	unsigned char mbr_sector_buf[512];
	ata_pio_read(0, 1, mbr_sector_buf);

	return *(struct mbr_partion_table*)(mbr_sector_buf + 0x1BE);
}


void print_mbr_partion_table(struct mbr_partion_table mbr_table) {
	char tempbuf[100];
	print_string("partion num | boot | start | end | sectors | ID\n");

	bool bootflag = false;
	uint64_t start_sector = 0;
	uint64_t total_sectors = 0;
	uint64_t end_sector = 0;
	uint64_t system_id = 0;
	for (uint8_t i = 0; i < MBR_MAX_PARTION_TABLE_COUNT; i++) {
		// Making sure that the entire entry is not empty:
		if (
			(*(uint64_t*)(&mbr_table.partions[i])) == 0 &&
			(*(uint64_t*)(((uint8_t*)&mbr_table.partions[i]) + 8)) == 0
		) continue; 

		bootflag = mbr_table.partions[i].drive_attributes & 0x80;
		start_sector = mbr_table.partions[i].relative_sectors_lba_start;
		total_sectors = mbr_table.partions[i].total_sectors;
		end_sector = start_sector + total_sectors - 1;
		system_id = mbr_table.partions[i].system_id;

		// partion num:
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

