#include <include/mem_utils.h>
#include <include/strings.h>
#include <kernel/include/heap.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "drivers/framebuffer_print.h"
#include "font.h"

struct framebuffer_info {
	uint64_t chars_per_row;
	uint64_t chars_per_column;
};

#define CHAR_PIXEL_WIDTH 8
#define CHAR_PIXEL_HEIGHT 16

static struct limine_framebuffer* g_framebuffer = NULL;
static uint32_t* g_framebuffer_addr = NULL;
static uint64_t g_framebuffer_height;
static uint64_t g_framebuffer_width;
static uint64_t g_curr_cursor_index = 0;
static struct char_device g_framebuffer_device;
static bool g_device_initialized = false;

static void print_framebuffer_response(
	volatile struct limine_framebuffer_response* framebuffer_response);
static void print_letter(char letter);
static void slide_framebuffer();

static ssize_t framebuffer_read(struct char_device* device, void* buffer, size_t size);
static ssize_t framebuffer_ioctl(struct char_device* device, uint32_t command, void* arg);
static ssize_t framebuffer_write(struct char_device* device, void* buffer, size_t size);

struct char_device* init_framebuffer_print(
	volatile struct limine_framebuffer_response* framebuffer_response) {
	KERNEL_ASSERT(framebuffer_response != NULL, "framebuffer response is NULL");
	KERNEL_ASSERT(framebuffer_response->framebuffer_count > 0, "No framebuffer found");

	if (g_device_initialized) {
		return &g_framebuffer_device;
	}

	g_framebuffer = framebuffer_response->framebuffers[0];
	g_framebuffer_addr = (uint32_t*)g_framebuffer->address;
	g_framebuffer_height = g_framebuffer->height;
	g_framebuffer_width = g_framebuffer->width;

	g_device_initialized = true;
	g_framebuffer_device = (struct char_device){
		.name = "framebuffer",
		.capabilities = CDC_WRITE_CAPABILITY,
		.private_data = NULL,
		.read = framebuffer_read,
		.write = framebuffer_write,
		.ioctl = framebuffer_ioctl,
	};
	return &g_framebuffer_device;
}

static ssize_t framebuffer_read(struct char_device* device, void* buffer, size_t size) {
	return -1;
}
static ssize_t framebuffer_ioctl(struct char_device* device, uint32_t command, void* arg) {
	return 0;
}

static ssize_t framebuffer_write(struct char_device* device, void* buffer, size_t size) {
	uint64_t chars_per_row = g_framebuffer_width / CHAR_PIXEL_WIDTH;
	uint64_t chars_per_col = g_framebuffer_height / CHAR_PIXEL_HEIGHT;

	for (size_t i = 0; i < size; i++) {
		switch (((char*)buffer)[i]) {
			case '\n':
				g_curr_cursor_index += chars_per_row;
				g_curr_cursor_index -= g_curr_cursor_index % chars_per_row;
				break;
			default:
				print_letter(((char*)buffer)[i]);
				g_curr_cursor_index++;
				break;
		}

		if (g_curr_cursor_index >= chars_per_row * chars_per_col) {
			slide_framebuffer();
			g_curr_cursor_index -= chars_per_row;
		}
	}
}

bool is_pixel_on(char c, int x, int y) {
	int bytes_per_row = (CHAR_PIXEL_WIDTH + 7) / 8;
	int byte_index = (y * bytes_per_row) + (x / 8);
	int bit_index = 7 - (x % 8);

	return font8x16[c][byte_index] & (1 << bit_index);
}

static void print_letter(char letter) {
	uint64_t chars_per_row = g_framebuffer_width / CHAR_PIXEL_WIDTH;
	uint64_t char_x = g_curr_cursor_index % chars_per_row;
	uint64_t char_y = g_curr_cursor_index / chars_per_row;

	for (int y = 0; y < CHAR_PIXEL_HEIGHT; y++) {
		for (int x = 0; x < CHAR_PIXEL_WIDTH; x++) {
			uint32_t pixel_color = is_pixel_on(letter, x, y) ? 0x00FF00 : 0x000000;
			uint64_t pixel_y = (char_y * CHAR_PIXEL_HEIGHT) + y;
			uint64_t pixel_x = (char_x * CHAR_PIXEL_WIDTH) + x;
			g_framebuffer_addr[(pixel_y * (g_framebuffer->pitch / 4)) + pixel_x] = pixel_color;
		}
	}
}

static void slide_framebuffer() {
	uint32_t pixels_per_row = g_framebuffer->pitch / 4;
	size_t pixels_to_copy = (g_framebuffer_height - CHAR_PIXEL_HEIGHT) * pixels_per_row;

	memcpy(g_framebuffer_addr, g_framebuffer_addr + (CHAR_PIXEL_HEIGHT * pixels_per_row),
		   pixels_to_copy * sizeof(uint32_t));

	// Clear the bottom row
	memset(g_framebuffer_addr + pixels_to_copy, 0,
		   CHAR_PIXEL_HEIGHT * pixels_per_row * sizeof(uint32_t));
}

static void print_framebuffer_response(
	volatile struct limine_framebuffer_response* framebuffer_response) {
	printk("Framebuffer count: %d framebuffers\n", framebuffer_response->framebuffer_count);
	printk("Framebuffer address: %x\n", framebuffer_response->framebuffers[0]->address);

	printk("Framebuffer width: %d\n", framebuffer_response->framebuffers[0]->width);
	printk("Framebuffer height: %d\n", framebuffer_response->framebuffers[0]->height);

	printk("framebuffer : %d\n", framebuffer_response->framebuffers[0]->bpp);
	printk("framebuffer pitch: %d\n", framebuffer_response->framebuffers[0]->pitch);
}
