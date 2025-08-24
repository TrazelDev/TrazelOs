#include <types.h>
#include <io.h>

struct DEVICE {
	unsigned short base;
	unsigned short dev_ctl;
};

void ata_lba_read(struct DEVICE* ctrl, uint64_t lba_addressing, uint64_t sectors_to_read, char* buf) {
	// setting up what sectors are read:
	uint8_t drive_register_value = ((lba_addressing >> 24) & 0x0F) | 0b11100000;
	outb(ctrl->base + 6, drive_register_value);

	outb(0xF1, NULL);

	outb(ctrl->base + 2, sectors_to_read);

	uint8_t sector_number_register_value = lba_addressing & 0x0FF;
	outb(ctrl->base + 3, sector_number_register_value);

	uint8_t cylinder_low_register_value = (lba_addressing >> 8) & 0xFF;
	outb(ctrl->base + 4, cylinder_low_register_value);

	uint8_t cylinder_high_register_value = (lba_addressing >> 16) & 0xFF;
	outb(ctrl->base + 5, cylinder_high_register_value);

	uint16_t command_port = ctrl->base + 7;
	outb(command_port, 0x20);

	
	// waiting until the sector buffer is ready: 
	while (!(inb(command_port) & 8));


	// Reading into the buffer:
	uint16_t data_port = ctrl->base;
	unsigned short* buffer = (unsigned short*)buf;
	for (uint64_t i = 0; i < sectors_to_read * 256; i++) {
		buffer[i] = inw(data_port);
	}
}
