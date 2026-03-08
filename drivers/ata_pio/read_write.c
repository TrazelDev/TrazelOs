#include <drivers/ata_pio.h>
#include <io.h>
// The following dcoumention was used: https://wiki.osdev.org/ATA_PIO_Mode
// in order to create this driver

struct DEVICE {
	unsigned short base;
	unsigned short dev_ctl;
};
static struct DEVICE s_ctrl;

enum base_registers_offset {
	DATA_REGISTER = 0,
	ERROR_OR_FEATURE_REGISTER = 1,
	SECTOR_COUNT_REGISTER = 2,
	SECTOR_NUMBER_REGISTER = 3,
	CYLINDER_LOW_REGISTER = 4,
	CYLINDER_HIGH_REGISTER = 5,
	DRIVE_OR_HEAD_REGISTER = 6,
	STATUS_OR_COMMAND_REGISTER = 7,
};

enum command_port_values {
	COMMAND_PORT_READ_VALUE = 0x20,
	COMMAND_PORT_WRITE_VALUE = 0x30,
};

/**
 * Sets up the sectors for read or write operations
 * lba_addressing - starting sector number that the operation will happen on
 * sectors_to_operate - amount of sectors that will be used in the read or write
 */
void ata_pio_prepare_for_operations(uint64_t lba_addressing, uint64_t sectors_to_operate);

void ata_pio_init() {
	s_ctrl.base = 0x1F0;
	s_ctrl.dev_ctl = 0x3F6;
}

void ata_pio_read(uint64_t lba_addressing, uint64_t sectors_to_read, char* buf) {
	ata_pio_prepare_for_operations(lba_addressing, sectors_to_read);

	// Selecting the read operation
	uint16_t command_port = s_ctrl.base + STATUS_OR_COMMAND_REGISTER;
	outb(command_port, COMMAND_PORT_READ_VALUE);

	// waiting until the sector buffer is ready:
	while (!(inb(command_port) & 8));

	// Reading into the buffer:
	uint16_t data_port = s_ctrl.base + DATA_REGISTER;
	unsigned short* sector_buffer = (unsigned short*)buf;
	for (uint64_t i = 0; i < sectors_to_read * 256; i++) {
		sector_buffer[i] = inw(data_port);
	}
}

void ata_pio_write(uint64_t lba_addressing, uint64_t sectors_to_write, char* buf) {
	ata_pio_prepare_for_operations(lba_addressing, sectors_to_write);

	// Selecting the read operation
	uint16_t command_port = s_ctrl.base + STATUS_OR_COMMAND_REGISTER;
	outb(command_port, COMMAND_PORT_WRITE_VALUE);

	// waiting until the sector buffer is ready:
	while (!(inb(command_port) & 8));

	// Reading into the buffer:
	uint16_t data_port = s_ctrl.base + DATA_REGISTER;
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
	if (buf[84] != 'G') {
		return false;
	}

	buf[0] = 'A';
	ata_pio_write(0, 1, buf);
	for (int i = 0; i < 512; i++) {
		buf[i] = 0;
	}

	ata_pio_read(0, 1, buf);
	if (buf[0] != 'A') {
		return false;
	}

	return true;
}

void ata_pio_prepare_for_operations(uint64_t lba_addressing, uint64_t sectors_to_operate) {
	uint8_t drive_register_value = ((lba_addressing >> 24) & 0x0F) | 0b11100000;
	outb(s_ctrl.base + DRIVE_OR_HEAD_REGISTER, drive_register_value);

	outb(s_ctrl.base + ERROR_OR_FEATURE_REGISTER, NULL);

	outb(s_ctrl.base + SECTOR_COUNT_REGISTER, sectors_to_operate);

	uint8_t sector_number_register_value = lba_addressing & 0x0FF;
	outb(s_ctrl.base + SECTOR_NUMBER_REGISTER, sector_number_register_value);

	uint8_t cylinder_low_register_value = (lba_addressing >> 8) & 0xFF;
	outb(s_ctrl.base + CYLINDER_LOW_REGISTER, cylinder_low_register_value);

	uint8_t cylinder_high_register_value = (lba_addressing >> 16) & 0xFF;
	outb(s_ctrl.base + CYLINDER_HIGH_REGISTER, cylinder_high_register_value);
}
