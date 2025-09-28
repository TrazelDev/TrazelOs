// TODO: IGNORE THIS for now this is long term
// 1. Read the following and decide what filing system you want:
// https://wiki.osdev.org/FAT you are in the implemention details section
// 2. Create a small filing system fat12 or BPB and find the kernel file
// 3. Create an elf loader to actually load the kernel
// 4. establish debugging insdie of the kernel using gdb


// TODO: Do this next
// 1.Make it so the master boot record will only support boot partions with FAT12
// so it will pull the ID from the table and if it not ID=1 which is FAT12 it will
// say that there is no bootable partion which is FAT12 which the operating system supports
// than it will instead of loading the entire partion it will load only the reserved sectors
// of the bootlaoder






#define REG_CYL_LO 4 
#define REG_CYL_HI 5
#define REG_DEVSEL 6


#define ATADEV_PATAPI 1
#define ATADEV_SATAPI 2
#define ATADEV_PATA 3
#define ATADEV_SATA 4
#define ATADEV_UNKNOWN 5


int ata_soft_reset(short dev_ctl) {
	outb(dev_ctl, )	
}
// on Primary bus: ctrl->base =0x1F0, ctrl->dev_ctl =0x3F6. REG_CYL_LO=4, REG_CYL_HI=5, REG_DEVSEL=6
int detect_devtype (int slavebit, struct DEVICE *ctrl)
{
	// 	ata_soft_reset(ctrl->dev_ctl); // waits until master drive is ready again 
	outb(ctrl->base + REG_DEVSEL, 0xA0 | slavebit<<4);
	inb(ctrl->dev_ctl);			//  wait 400ns for drive select to work 
	inb(ctrl->dev_ctl);
	inb(ctrl->dev_ctl);
	inb(ctrl->dev_ctl);
	unsigned cl = inb(ctrl->base + REG_CYL_LO);	// get the "signature bytes"
	unsigned ch = inb(ctrl->base + REG_CYL_HI);

	// differentiate ATA, ATAPI, SATA and SATAPI 
	if (cl==0x14 && ch==0xEB) return ATADEV_PATAPI;
	if (cl==0x69 && ch==0x96) return ATADEV_SATAPI;
	if (cl==0 && ch == 0) return ATADEV_PATA;
	if (cl==0x3c && ch==0xc3) return ATADEV_SATA;
	return ATADEV_UNKNOWN;
}

void test_detect_devtype() {
	struct DEVICE ctrl;
	ctrl.base = 0x1F0;
	ctrl.dev_ctl = 0x3F6;
	int slavebit = 0;
 
	int val = detect_devtype(slavebit, &ctrl);

	// if (val == ATADEV_PATAPI) print_string("ATADEV_PATAPI");
	// if (val == ATADEV_SATAPI) print_string("ATADEV_SATAPI");
	// if (val == ATADEV_PATA) print_string("ATADEV_PATA");
	// if (val == ATADEV_SATA) print_string("ATADEV_SATA");
	// if (val == ATADEV_UNKNOWN) print_string("ATADEV_UNKNOWN");
}




// outb(0x1F6, 0xE0 | (0 << 4) | ((lba_addressing >> 24) & 0x0F));


/*
void ata_lba_read(struct DEVICE* ctrl, uint64_t lba_addressing, uint8_t sectors_to_read, char* buf) {

// Wait for BSY to clear while (inb(ctrl->base + 7) & 0x80);

	// (Optionally) Select drive
	outb(ctrl->base + 6, 0xE0); // Master, LBA mode

	// (Optionally) Wait again for BSY to clear
	while (inb(ctrl->base + 7) & 0x80);

    // Setup registers (as you do, with masking)
    outb(ctrl->base + 6, ((lba_addressing >> 24) & 0x0F) | 0xE0);
    outb(ctrl->base + 2, sectors_to_read);
    outb(ctrl->base + 3, lba_addressing & 0xFF);
    outb(ctrl->base + 4, (lba_addressing >> 8) & 0xFF);
    outb(ctrl->base + 5, (lba_addressing >> 16) & 0xFF);
    outb(ctrl->base + 7, 0x20);

    uint16_t data_port = ctrl->base;

    for (uint64_t s = 0; s < sectors_to_read; s++) {
        // Wait for DRQ (add timeout for robustness)
        int timeout = 100000;
        while (!(inb(ctrl->base + 7) & 8) && --timeout);
        if (timeout == 0) {
            // handle error (timeout or return)
            return;
        }
        // Read 512 bytes
        for (int i = 0; i < 512; i++) {
            buf[s*512 + i] = inb(data_port);
        }
    }
}
*/



// Remmber whats good about lba is that you just specify the sector that you 
// want to read, if you want to read sector 20318 the lba_sector will have the
// value 20318 instead of someting complicated with CHS where you need to
// calcluate it with 3 different values, the reason CHS was used in the first place
// was because originally the max register size was only 256
// TODO: Add this explanation to obsidian

