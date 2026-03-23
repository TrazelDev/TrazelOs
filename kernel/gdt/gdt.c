#include <include/mem_utils.h>
#include <kernel/include/panic.h>

#include "kernel/include/gdt.h"
#include "kernel/include/printk.h"

#define GDT_NULL_SEGMENT 0
#define GDT_CODE_SEGMENT 1
#define GDT_DATA_SEGMENT 2

/* This struct is based on the gdt.asm in bootloader more precise documentation is in there */
struct gdt_entry {
	uint16_t limit;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
} __attribute__((packed));

struct gdt_desc {
	uint16_t limit;
	size_t base;
} __attribute__((packed));

static struct gdt_entry g_gdt_entries[3];
static struct gdt_desc g_gdt_descriptor;

extern void gdt_reload_segments(uint16_t code_sel, uint16_t data_sel);
void init_gdt() {
	memset(&g_gdt_entries[GDT_NULL_SEGMENT], 0, sizeof(struct gdt_entry));

	g_gdt_entries[GDT_CODE_SEGMENT].limit = 0;
	g_gdt_entries[GDT_CODE_SEGMENT].base_low = 0;
	g_gdt_entries[GDT_CODE_SEGMENT].base_mid = 0;
	g_gdt_entries[GDT_CODE_SEGMENT].access = 0x9A;
	g_gdt_entries[GDT_CODE_SEGMENT].granularity = 0x20;
	g_gdt_entries[GDT_CODE_SEGMENT].base_high = 0;

	g_gdt_entries[GDT_DATA_SEGMENT].limit = 0;
	g_gdt_entries[GDT_DATA_SEGMENT].base_low = 0;
	g_gdt_entries[GDT_DATA_SEGMENT].base_mid = 0;
	g_gdt_entries[GDT_DATA_SEGMENT].access = 0x92;
	g_gdt_entries[GDT_DATA_SEGMENT].granularity = 0x00;
	g_gdt_entries[GDT_DATA_SEGMENT].base_high = 0;

	g_gdt_descriptor =
		(struct gdt_desc){.limit = (sizeof(g_gdt_entries) - 1), .base = (size_t)&g_gdt_entries};

	asm volatile("lgdt %0" : : "m"(g_gdt_descriptor));

	uint16_t code_sel = GDT_CODE_SEGMENT << 3;
	uint16_t data_sel = GDT_DATA_SEGMENT << 3;
	gdt_reload_segments(code_sel, data_sel);

	printk("Initialized GDT\n");
}

uint8_t get_segment_index(enum gdt_segments segment) {
	switch (segment) {
		case GS_CODE_SEGMENT:
			return GDT_CODE_SEGMENT;
		default:
			KERNEL_PANIC("Invalid GDT segment");
	}
}
