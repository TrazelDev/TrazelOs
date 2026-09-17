#include <include/mem_utils.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "gdt_defs.h"
#include "kernel/include/gdt.h"

#define KERNEL_STACK_SIZE 4096
#define GDT_ENTRIES 6
#define GDT_NULL_SEGMENT 0
#define GDT_RING0_CODE_SEGMENT 1
#define GDT_RING0_DATA_SEGMENT 2
#define GDT_RING3_DATA_SEGMENT 3
#define GDT_RING3_CODE_SEGMENT 4
#define GDT_TSS_SEGMENT 5

// We use +1 cause the tss descriptor is twice as big
static struct gdt_entry g_gdt_entries[GDT_ENTRIES + 1];
static struct gdt_desc g_gdt_descriptor;
static uint8_t g_tss_ring0_stack[KERNEL_STACK_SIZE];
static struct tss_entry g_tss;

static void setup_gdt_entry(struct gdt_entry* gdt_entry, bool is_code_segment, bool is_ring3);
static void setup_tss_gdt_entry(struct gdt_entry* gdt_entry, struct tss_entry* tss_entry);

extern void gdt_reload_segments(uint16_t code_sel, uint16_t data_sel);
void init_gdt() {
	memset(&g_gdt_entries[GDT_NULL_SEGMENT], 0, sizeof(struct gdt_entry));

	setup_gdt_entry(&g_gdt_entries[GDT_RING0_CODE_SEGMENT], true, false);
	setup_gdt_entry(&g_gdt_entries[GDT_RING0_DATA_SEGMENT], false, false);
	setup_gdt_entry(&g_gdt_entries[GDT_RING3_DATA_SEGMENT], false, true);
	setup_gdt_entry(&g_gdt_entries[GDT_RING3_CODE_SEGMENT], true, true);
	setup_tss_gdt_entry(&g_gdt_entries[GDT_TSS_SEGMENT], &g_tss);

	g_gdt_descriptor =
		(struct gdt_desc){.limit = (sizeof(g_gdt_entries) - 1), .base = (size_t)&g_gdt_entries};

	asm volatile("lgdt %0" : : "m"(g_gdt_descriptor));

	uint16_t code_sel = GDT_RING0_CODE_SEGMENT << 3;
	uint16_t data_sel = GDT_RING0_DATA_SEGMENT << 3;
	gdt_reload_segments(code_sel, data_sel);

	asm volatile("ltr %0" : : "r"((uint16_t)(GDT_TSS_SEGMENT << 3)));

	printk("Initialized GDT\n");
}

uint16_t gdt_get_segment_index(enum gdt_segments segment) {
	switch (segment) {
		case GS_RING0_CODE_SEGMENT:
			return GDT_RING0_CODE_SEGMENT;
		case GS_RING0_DATA_SEGMENT:
			return GDT_RING0_DATA_SEGMENT;
		case GS_RING3_CODE_SEGMENT:
			return GDT_RING3_CODE_SEGMENT;
		case GS_RING3_DATA_SEGMENT:
			return GDT_RING3_DATA_SEGMENT;

		default:
			KERNEL_PANIC("Invalid GDT segment");
	}
}

// module private functions:
// -------------------------------------------------------------------------------------------------

static void setup_gdt_entry(struct gdt_entry* gdt_entry, bool is_code_segment, bool is_ring3) {
	gdt_entry->limit_low = 0xFFFF;
	gdt_entry->limit_high = 0xF;
	gdt_entry->base_low = 0;
	gdt_entry->base_mid = 0;
	gdt_entry->base_high = 0;

	// (0x9A) 0b10011010 (code descriptor ring0)
	// (0x92) 0b10010010 (data descriptor ring0)
	// (0xF2) 0b11110010 (data ring3)
	// (0xFA) 0b11111010 (code ring3)
	gdt_entry->access.access = 0;
	gdt_entry->access.read_write = 1;
	gdt_entry->access.direction_conforming = 0;
	gdt_entry->access.executable = is_code_segment;
	gdt_entry->access.descriptor_type = 1;
	gdt_entry->access.descriptor_privilege_level = is_ring3 ? 3 : 0;
	gdt_entry->access.present = 1;

	// (0xA) 0b1010 (code descriptor)
	// (0xC) 0b1100 (data descriptor)
	gdt_entry->reserved = 0;
	gdt_entry->long_mode = is_code_segment;
	gdt_entry->size_flag = !is_code_segment;
	gdt_entry->granularity = 1;
}

// replace with the macros
#define GET_TSS_LOW_BIT_MASK(tss_entry) ((uint16_t)((size_t)(tss_entry) & 0xFFFF))
#define GET_TSS_MID_BIT_MASK(tss_entry) ((uint8_t)(((size_t)(tss_entry) >> 16) & 0xFF))
#define GET_TSS_HIGH_BIT_MASK(tss_entry) ((uint8_t)(((size_t)(tss_entry) >> 24) & 0xFF))
#define GET_TOP32_BIT_MASK(tss_entry) ((size_t)(tss_entry) >> 32)

static void setup_tss_gdt_entry(struct gdt_entry* gdt_entry, struct tss_entry* tss_entry) {
	memset(&g_tss, 0, sizeof(struct tss_entry));
	g_tss.rsp0 = (uint64_t)(g_tss_ring0_stack + KERNEL_STACK_SIZE);
	g_tss.iopb_offset = sizeof(struct tss_entry);

	// limit:
	gdt_entry->limit_low = (sizeof(struct tss_entry) & 0xFFFF) - 1;
	gdt_entry->limit_high = sizeof(struct tss_entry) >> 16;

	uint64_t* base_top32 = (uint64_t*)(gdt_entry + 1);
	gdt_entry->base_low = GET_TSS_LOW_BIT_MASK(tss_entry);
	gdt_entry->base_mid = GET_TSS_MID_BIT_MASK(tss_entry);
	gdt_entry->base_high = GET_TSS_HIGH_BIT_MASK(tss_entry);
	*base_top32 = GET_TOP32_BIT_MASK(tss_entry);

	// 0x89 0b10001001
	gdt_entry->access.access = 1;
	gdt_entry->access.read_write = 0;
	gdt_entry->access.direction_conforming = 0;
	gdt_entry->access.executable = 1;
	gdt_entry->access.descriptor_type = 0;
	gdt_entry->access.descriptor_privilege_level = 0b00;
	gdt_entry->access.present = 1;

	gdt_entry->reserved = 0;
	gdt_entry->long_mode = 0;
	gdt_entry->size_flag = 0;
	gdt_entry->granularity = 0;
}
