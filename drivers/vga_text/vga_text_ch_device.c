#include <include/mem_utils.h>
#include <include/strings.h>
#include <include/types.h>

#include "drivers/vga_text.h"
#include "io.h"

enum special_chars { NEW_LINE = '\n', BACK_SPACE = '\b' };
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((uint8_t*)0xB8000)
#define VGA_LAST_MEMORY_ADDRESS ((uint8_t*)(0xB8000 + (VGA_WIDTH * VGA_HEIGHT * 2)))

struct vga_data {
	uint64_t vga_width;
	uint64_t vga_height;
	uint8_t* starting_vga_memory_addr;
	uint8_t* final_vga_memory_addr;
	uint64_t cursor_position;
};

static struct char_device g_char_device;
static struct vga_data g_vga_data;
static char* g_device_name = "vga_text";
static struct vga_data* get_private_vga_data(struct char_device* device) {
	return (struct vga_data*)(device->private_data);
}

/*
 * Takes x,y coordinates and translates to the correct vga text mode position
 * x - max of VGA_WIDTH
 * y - max of VGA_HEIGHT
 * returns - the correct position the the function set_cursor_position takes
 */
static uint16_t position_to_coordinates(uint8_t x, uint8_t y);
static void set_cursor_position(uint16_t position);
static void output_char(char output_ch, uint8_t* starting_vga_mem, uint32_t offset);

static ssize_t vga_text_read(struct char_device* device, void* buffer, size_t size);
static ssize_t vga_text_write(struct char_device* device, void* buffer, size_t size);
static ssize_t vga_text_ioctl(struct char_device* device, uint32_t command, void* arg);
struct char_device* vga_text_init() {
	static bool is_initiazlied = false;
	if (is_initiazlied) {
		return &g_char_device;
	}

	is_initiazlied = true;
	g_vga_data.cursor_position = 0;
	g_vga_data.vga_width = VGA_WIDTH;
	g_vga_data.vga_height = VGA_HEIGHT;
	g_vga_data.starting_vga_memory_addr = VGA_MEMORY;
	g_vga_data.final_vga_memory_addr = VGA_LAST_MEMORY_ADDRESS;

	g_char_device.private_data = (void*)&g_vga_data;
	memcpy(g_char_device.name, g_device_name, strlen(g_device_name) + 1);
	g_char_device.write = vga_text_write;
	g_char_device.read = vga_text_read;
	g_char_device.ioctl = vga_text_ioctl;

	set_cursor_position(position_to_coordinates(0, 0));
	return &g_char_device;
}

static ssize_t vga_text_read(struct char_device* device, void* buffer, size_t size) { return -1; }
static ssize_t vga_text_ioctl(struct char_device* device, uint32_t command, void* arg) { return 0; }
static ssize_t vga_text_write(struct char_device* device, void* buffer, size_t size) {
	struct vga_data* vga_data = get_private_vga_data(device);
	uint16_t curr_cursor_index = vga_data->cursor_position;
	char* str = (char*)buffer;

	ssize_t i = 0;
	for (i = 0; i < size; i++) {
		switch (str[i]) {
			case NEW_LINE:
				curr_cursor_index += vga_data->vga_width;
				curr_cursor_index -= (curr_cursor_index % vga_data->vga_width + 1);
				break;
			case BACK_SPACE:
				curr_cursor_index--;
				output_char(' ', VGA_MEMORY, curr_cursor_index);
				curr_cursor_index--;
				break;
			default:
				output_char(str[i], VGA_MEMORY, curr_cursor_index);
				break;
		}
		curr_cursor_index++;
	}

	vga_data->cursor_position = curr_cursor_index;
	set_cursor_position(curr_cursor_index);
	return i;
}

static void output_char(char output_ch, uint8_t* starting_vga_mem, uint32_t offset) {
	*(starting_vga_mem + ((size_t)(offset * 2))) = output_ch;
}
static uint64_t global_cursor_position = 0;

static uint16_t position_to_coordinates(uint8_t x, uint8_t y) { return (y * VGA_WIDTH) + x; }
static void set_cursor_position(uint16_t position) {
#define POSITION_LOWER_BYTES(position) ((uint8_t)((position) & 0xFF))
#define POSITION_UPPER_BYTES(position) ((uint8_t)(((position) >> 8) & 0xFF))

	// tell the VGA controller that we're setting the low byte of the cursor position
	outb(IO_VGA_REGISTER_PORT, 0x0F);

	// set the low byte of the cursor position
	outb(IO_VGA_DATA_PORT, POSITION_LOWER_BYTES(position));

	// tell the VGA controller that we're setting the high byte of the cursor position
	outb(IO_VGA_REGISTER_PORT, 0x0E);

	// set the high byte of the cursor position
	outb(IO_VGA_DATA_PORT, POSITION_UPPER_BYTES(position));

	// updating the position variable:
	global_cursor_position = position;

#undef POSITION_LOWER_BYTES
#undef POSITION_UPPER_BYTES
}
