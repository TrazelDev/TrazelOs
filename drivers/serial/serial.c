// When you want to refactor this code use this: https://wiki.osdev.org/Serial_Ports
// This code is just rapid prototype and will be refactored when needed.
#include <drivers/serial.h>
#include <include/io.h>
#include <include/mem_utils.h>
#include <include/strings.h>
#include <include/types.h>

#define PORT 0x3f8
static struct char_device g_char_device;
static char* g_device_name = "com1";
static ssize_t com1_read(struct char_device* device, void* buffer, size_t size);
static ssize_t com1_write(struct char_device* device, void* buffer, size_t size);
static ssize_t com1_ioctl(struct char_device* device, uint32_t command, void* arg);
static int is_transmit_empty();
static int init_com1();
static void transmit_char(char chr);
struct char_device* serial_com1_init() {
	static bool is_initiazlied = false;
	if (is_initiazlied) {
		return &g_char_device;
	}

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
	outb(PORT + 1, 0x00);  // Disable all interrupts
	outb(PORT + 3, 0x80);  // Enable DLAB (set baud rate divisor)
	outb(PORT + 0, 0x03);  // Set divisor to 3 (lo byte) 38400 baud
	outb(PORT + 1, 0x00);  //                  (hi byte)
	outb(PORT + 3, 0x03);  // 8 bits, no parity, one stop bit
	outb(PORT + 2, 0xC7);  // Enable FIFO, clear them, with 14-byte threshold
	outb(PORT + 4, 0x0B);  // IRQs enabled, RTS/DSR set
	outb(PORT + 4, 0x1E);  // Set in loopback mode, test the serial chip
	outb(PORT + 0,
		 0xAE);	 // Test serial chip (send byte 0xAE and check if serial returns same byte)

	// Check if serial is faulty (i.e: not same byte as sent)
	if (inb(PORT + 0) != 0xAE) {
		return -1;
	}

	// If serial is not faulty set it in normal operation mode
	// (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
	outb(PORT + 4, 0x0F);
	return 0;
}

static int is_transmit_empty() { return inb(PORT + 5) & 0x20; }

static void transmit_char(char chr) {
	while (is_transmit_empty() == 0);

	outb(PORT, chr);
}
static ssize_t com1_write(struct char_device* device, void* buffer, size_t size) {
	char* src_buffer = (char*)buffer;

	for (size_t i = 0; i < size; i++) {
		transmit_char(src_buffer[i]);
	}

	return (ssize_t)size;
}
