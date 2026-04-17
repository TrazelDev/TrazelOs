// When you want to refactor this code use this: https://wiki.osdev.org/Serial_Ports
// This code is just rapid prototype and will be refactored when needed.
#include <drivers/serial.h>
#include <include/io.h>
#include <include/mem_utils.h>
#include <include/strings.h>
#include <include/types.h>

enum com_ports {
	COM1_PORT = 0x3F8,
	COM2_PORT = 0x2F8,
	COM3_PORT = 0x3E8,
	COM4_PORT = 0x2E8,
	COM5_PORT = 0x5F8,
	COM6_PORT = 0x4F8,
	COM7_PORT = 0x5E8,
	COM8_PORT = 0x4E8,
};
enum serial_commands {
	SC_BUFFER = 0,
	SC_INTERRUPT_ENABLE_REGISTER = 1,
	SC_INTERRUPT_IDENTIFICATION = 2,
	SC_FIFO_CONTROL_REGISTERS = 2,
	SC_LINE_CONTROL_REGISTER = 3,
	SC_MODEM_CONTROL_REGISTER = 4,
	SC_LINE_STATUS_REGISTER = 5,
	SC_MODEM_STATUS_REGISTER = 6,
	SC_SCRATCH_REGISTER = 7,
};

static struct char_device g_char_device;
static char* g_device_name = "com1";
static ssize_t com1_read(struct char_device* device, void* buffer, size_t size);
static ssize_t com1_write(struct char_device* device, void* buffer, size_t size);
static ssize_t com1_ioctl(struct char_device* device, uint32_t command, void* arg);
static int is_transmit_empty();
static int init_com1();
static void transmit_char(char chr);
struct char_device* serial_com1_init() {
	static bool is_initialized = false;
	if (is_initialized) {
		return &g_char_device;
	}
	is_initialized = true;

	memcpy(g_char_device.name, g_device_name, strlen(g_device_name) + 1);
	g_char_device.write = com1_write;
	g_char_device.read = com1_read;
	g_char_device.ioctl = com1_ioctl;

	if (init_com1() == -1) {
		return NULL;
	}
	return &g_char_device;
}

static ssize_t com1_read(struct char_device* device, void* buffer, size_t size) { return -1; }
static ssize_t com1_ioctl(struct char_device* device, uint32_t command, void* arg) { return 0; }

static int init_com1() {
	outb(COM1_PORT + 1, 0x00);	// Disable all interrupts
	outb(COM1_PORT + 3, 0x80);	// Enable DLAB (set baud rate divisor)
	outb(COM1_PORT + 0, 0x03);	// Set divisor to 3 (lo byte) 38400 baud
	outb(COM1_PORT + 1, 0x00);	//                  (hi byte)
	outb(COM1_PORT + 3, 0x03);	// 8 bits, no parity, one stop bit
	outb(COM1_PORT + 2, 0xC7);	// Enable FIFO, clear them, with 14-byte threshold
	outb(COM1_PORT + 4, 0x0B);	// IRQs enabled, RTS/DSR set
	outb(COM1_PORT + 4, 0x1E);	// Set in loopback mode, test the serial chip
	outb(COM1_PORT + 0,
		 0xAE);	 // Test serial chip (send byte 0xAE and check if serial returns same byte)

	// Check if serial is faulty (i.e: not same byte as sent)
	if (inb(COM1_PORT + 0) != 0xAE) {
		return -1;
	}

	// If serial is not faulty set it in normal operation mode
	// (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
	outb(COM1_PORT + 4, 0x0F);
	return 0;
}

static int is_transmit_empty() { return inb(COM1_PORT + 5) & 0x20; }

static void transmit_char(char chr) {
	while (is_transmit_empty() == 0);

	outb(COM1_PORT, chr);
}
static ssize_t com1_write(struct char_device* device, void* buffer, size_t size) {
	char* src_buffer = (char*)buffer;

	for (size_t i = 0; i < size; i++) {
		transmit_char(src_buffer[i]);
	}

	return (ssize_t)size;
}
