#include <drivers/ata_pio.h>
#include <io.h>
// The following dcoumention was used: https://wiki.osdev.org/ATA_PIO_Mode
// in order to create this driver


struct DEVICE {
	unsigned short base;
	unsigned short dev_ctl;
};
static struct DEVICE s_ctrl;

enum BASE_REGISTERS_OFFSET {
	data_register = 0,
	error_or_feature_register = 1,
	sector_count_register = 2,
	sector_numer_register = 3,
	cylinder_low_register = 4,
	cylinder_high_register = 5,
	drive_or_head_register = 6,
	status_or_command_register = 7,
};

enum COMMAND_PORT_VALUES {
	command_port_read_value = 0x20,
	command_port_write_value = 0x30,
};

/**
 * Sets up the sectors for read or write operations
 * lba_addressing - starting sector number that the operation will happen on
 * sectors_to_operate - amount of sectors that will be used in the read or write 
*/
void ata_pio_prepare_for_opeartions(uint64_t lba_addressing, uint64_t sectors_to_operate);



void ata_pio_init() {
	s_ctrl.base = 0x1F0;
	s_ctrl.dev_ctl = 0x3F6;
}


void ata_pio_read(uint64_t lba_addressing, uint64_t sectors_to_read, char* buf) {
	ata_pio_prepare_for_opeartions(lba_addressing, sectors_to_read);
	
	// Selecting the read operation
	uint16_t command_port = s_ctrl.base + status_or_command_register;
	outb(command_port, command_port_read_value);

	
	// waiting until the sector buffer is ready: 
	while (!(inb(command_port) & 8));


	// Reading into the buffer:
	uint16_t data_port = s_ctrl.base + data_register;
	unsigned short* sector_buffer = (unsigned short*)buf;
	for (uint64_t i = 0; i < sectors_to_read * 256; i++) {
		sector_buffer[i] = inw(data_port);
	}
}

void ata_pio_write(uint64_t lba_addressing, uint64_t sectors_to_write, char* buf) {
	ata_pio_prepare_for_opeartions(lba_addressing, sectors_to_write);
	
	// Selecting the read operation
	uint16_t command_port = s_ctrl.base + status_or_command_register;
	outb(command_port, command_port_write_value);

	
	// waiting until the sector buffer is ready: 
	while (!(inb(command_port) & 8));


	// Reading into the buffer:
	uint16_t data_port = s_ctrl.base + data_register;
	unsigned short* sector_buffer = (unsigned short*)buf;
	for (uint64_t i = 0; i < sectors_to_write * 256; i++) {
		outw(data_port, sector_buffer[i]);
	}
}


bool ata_pio_min_test() {
	char buf[512];
	for (int i = 0; i < 512; i++) {
		buf[i] = 0;
	}

	ata_pio_read(0, 1, buf);
	if (buf[84] != 'G') return false;


	buf[0] = 'A';
	ata_pio_write(0, 1, buf);
	for (int i = 0; i < 512; i++) {
		buf[i] = 0;
	}

	ata_pio_read(0, 1, buf);
	if (buf[0] != 'A') return false;

	return true;
}

void ata_pio_prepare_for_opeartions(uint64_t lba_addressing, uint64_t sectors_to_operate) {
	uint8_t drive_register_value = ((lba_addressing >> 24) & 0x0F) | 0b11100000;
	outb(s_ctrl.base + drive_or_head_register, drive_register_value);

	outb(s_ctrl.base + error_or_feature_register, NULL);

	outb(s_ctrl.base + sector_count_register, sectors_to_operate);

	uint8_t sector_number_register_value = lba_addressing & 0x0FF;
	outb(s_ctrl.base + sector_numer_register, sector_number_register_value);

	uint8_t cylinder_low_register_value = (lba_addressing >> 8) & 0xFF;
	outb(s_ctrl.base + cylinder_low_register, cylinder_low_register_value);

	uint8_t cylinder_high_register_value = (lba_addressing >> 16) & 0xFF;
	outb(s_ctrl.base + cylinder_high_register, cylinder_high_register_value);
}
