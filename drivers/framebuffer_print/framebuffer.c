#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "drivers/framebuffer_print.h"

struct framebuffer_info {
	uint64_t chars_per_row;
	uint64_t chars_per_column;
};

#define CHAR_PIXEL_WIDTH 12
#define CHAR_PIXEL_HEIGHT 24
static const char A_BITS[] = {
	0x00, 0x00, 0x3C, 0x00, 0x38, 0x00, 0x50, 0x00, 0x50, 0x00, 0x48, 0x00, 0x88, 0x00, 0x88, 0x00,
	0xFC, 0x00, 0xFC, 0x01, 0x04, 0x01, 0x02, 0x01, 0x8F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static struct limine_framebuffer* g_framebuffer = NULL;
static uint32_t* g_framebuffer_addr = NULL;

static void print_framebuffer_response(
	volatile struct limine_framebuffer_response* framebuffer_response);
static void print_a();

bool is_pixel_on(char c, int x, int y) {
	int bytes_per_row = (CHAR_PIXEL_WIDTH + 7) / 8;
	int byte_index = (y * bytes_per_row) + (x / 8);
	int bit_index = x % 8;

	return A_BITS[byte_index] & (1 << bit_index);
}

void init_framebuffer_print(volatile struct limine_framebuffer_response* framebuffer_response) {
	KERNEL_ASSERT(framebuffer_response != NULL, "framebuffer response is NULL");
	KERNEL_ASSERT(framebuffer_response->framebuffer_count > 0, "No framebuffer found");

	g_framebuffer = framebuffer_response->framebuffers[0];
	g_framebuffer_addr = (uint32_t*)g_framebuffer->address;
	print_a();
}

static void print_a() {
	for (int y = 0; y < CHAR_PIXEL_HEIGHT; y++) {
		for (int x = 0; x < CHAR_PIXEL_WIDTH; x++) {
			if (is_pixel_on('A', x, y)) {
				g_framebuffer_addr[y * (g_framebuffer->pitch / 4) + x] = 0x00FF00;
			} else {
				g_framebuffer_addr[y * (g_framebuffer->pitch / 4) + x] = 0x000000;
			}
		}
	}
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
