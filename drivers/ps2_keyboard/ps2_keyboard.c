#include <include/io.h>
#include <include/ring_buffer.h>
#include <include/types.h>
#include <kernel/include/apic.h>
#include <kernel/include/intrrupts.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "drivers/ps2_keyboard.h"
#include "types.h"

const char SCAN_CODE_LOOKUP_TABLE[] = {
	0,	 0,	  '1', '2', '3', '4', '5', '6', '7', '8', '9',	'0', '-', '=',	'\b',
	0,	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',	'[', ']', '\n', '\n',
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,	  '\\', 'z',
	'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,	  '*',	0,	 ' ',
};

#define RING_BUFFER_SIZE 2056
struct ps2_keyboard_data {
	uint8_t s_buffer[RING_BUFFER_SIZE];
	struct ring_buffer s_ps2keyboard_rb;
};

static bool is_initialized = false;
static struct ps2_keyboard_data s_ps2_keyboard_data;
static struct char_device s_keyboard_device;

static ssize_t ps2_read(struct char_device* device, void* buffer, size_t size);
static ssize_t ps2_write(struct char_device* device, void* buffer, size_t size);
static ssize_t ps2_ioctl(struct char_device* device, uint32_t command, void* arg);

static inline struct ps2_keyboard_data* get_device_private_data(struct char_device* device);
static void keyboard_interrupt_handler(struct interrupt_info* info);

struct char_device* ps2_keyboard_init() {
	if (is_initialized) {
		return &s_keyboard_device;
	}

	// Creating the char device interface:
	is_initialized = true;
	ring_buffer_init(&s_ps2_keyboard_data.s_ps2keyboard_rb, s_ps2_keyboard_data.s_buffer,
					 RING_BUFFER_SIZE);
	s_keyboard_device = (struct char_device){
		.name = "ps2_keyboard",
		.capabilities = CDC_READ_CAPABILITY,
		.private_data = &s_ps2_keyboard_data,
		.read = ps2_read,
		.write = ps2_write,
		.ioctl = ps2_ioctl,
	};

	// creating the char device
	uint8_t keyboard_desc = get_free_interrupt_desc();
	apic_set_legacy_irq_desc_num(LIRQ_KEYBOARD, keyboard_desc);
	set_hardware_interrupt_handler(keyboard_desc, keyboard_interrupt_handler);

	printk("scan code lookup table size: %d\n", sizeof(SCAN_CODE_LOOKUP_TABLE));
	return &s_keyboard_device;
}

static inline struct ps2_keyboard_data* get_device_private_data(struct char_device* device) {
	return (struct ps2_keyboard_data*)device->private_data;
}

static ssize_t ps2_read(struct char_device* device, void* buffer, size_t size) {
	struct ps2_keyboard_data* data = get_device_private_data(device);
	char input_char;

	for (uint64_t i = 0; i < size; i++) {
		while (!ring_buffer_pop(&data->s_ps2keyboard_rb, (uint8_t*)&input_char)) {
			asm volatile("sti");
			asm volatile("hlt");
			asm volatile("cli");
		}
		((char*)buffer)[i] = input_char;
	}

	return (ssize_t)size;
}

static ssize_t ps2_write(struct char_device* device, void* buffer, size_t size) {
	KERNEL_PANIC("PS/2 Keyboard driver does not support write operation.");
}

static ssize_t ps2_ioctl(struct char_device* device, uint32_t command, void* arg) {
	KERNEL_PANIC("PS/2 Keyboard driver did not implement ioctl function yet");
}

static void keyboard_interrupt_handler(struct interrupt_info* info) {
	uint8_t scan_code = inb(IO_KEYBOARD_DATA_PORT);

	if (scan_code >= sizeof(SCAN_CODE_LOOKUP_TABLE)) {
		apic_send_eoi();
		return;
	}

	char input_char = SCAN_CODE_LOOKUP_TABLE[scan_code];
	ring_buffer_push(&s_ps2_keyboard_data.s_ps2keyboard_rb, input_char);
	apic_send_eoi();
}
