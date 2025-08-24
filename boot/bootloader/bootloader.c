#include <drivers/vga_text.h>
#include <types.h>


struct DEVICE {
	unsigned short base;
	unsigned short dev_ctl;
};
void ata_lba_read(struct DEVICE* ctrl, uint64_t lba_addressing, uint64_t sectors_to_read, char* buf);
void test_detect_devtype();

void bootloader_entry() {
	set_cursor_position(position_to_cordinates(0, 0));
	print_string("Hello world from bootloader\n");
	
	// test_detect_devtype();
	char buf[512];
	struct DEVICE ctrl;
	ctrl.base = 0x1F0;
	ctrl.dev_ctl = 0x3F6;
	
	for (int i = 0; i < 512; i++) {
		buf[i] = 0;
	}

	ata_lba_read(&ctrl, 0, 1, buf);
	if (buf[84] == 'G') print_string("success you managed to from the hard drive");


	asm volatile("hlt");
}
