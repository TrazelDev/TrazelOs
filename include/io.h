#pragma once
#include <types.h>

#define DISABLE_INTERRUPTS() asm volatile("cli")
#define ENABLE_INTERRUPTS()  asm volatile("sti")

void outb(unsigned short port, unsigned char val);
unsigned char inb(unsigned short port);
unsigned short inw(unsigned short port);

enum IO_PORTS
{
	// master pic ports:
	IO_master_pic_command_port = 0x20,
	IO_master_pic_data_port = 0x21,

	// slave pic ports:
	IO_slave_pic_command_port = 0xA0,
	IO_slave_pic_data_port = 0xA1,



	// keyboard:
	IO_keyboard_data_port = 0x60, // used to read data from the keyboard after a keyboard interrupt



	//CMOS Address Port (Port 0x70): Specifies the address (register index) of the Real-Time Clock (RTC) or CMOS configuration register.
	IO_cmos_address_port = 0x70,
	// CMOS Data Port (Port 0x71): Used for reading from or writing to the RTC or CMOS configuration register specified by the CMOS Address Port.
	IO_cmos_data_port = 0x71,



	// This port is used to select the VGA register that you want to read from or write to. 
	// The content written to this port specifies which VGA register (0-7) you are accessing. 
	// Once you have selected a register using this port, you use the second port (0x3D5) to read from or write to the data in that register.
	IO_vga_register_port = 0x3d4,
	// This port is used to read from or write to the VGA register selected by the previous write to port 0x3D4. 
	// If you write to port 0x3D5, you write data to the selected register, and if you read from port 0x3D5, you read data from the selected register.
	IO_vga_data_port = 0x3D5,


	// ata primary port this is the port the the offsets from it are all of the ways to to interact with the main hard disk:
	IO_ata_primary_device_port = 0x1F0
};
