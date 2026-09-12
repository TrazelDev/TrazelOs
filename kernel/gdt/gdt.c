#include <include/mem_utils.h>
#include <kernel/include/panic.h>

#include "kernel/include/gdt.h"
#include "kernel/include/printk.h"

#define KERNEL_STACK_SIZE 4096
#define GDT_ENTRIES 6
#define GDT_NULL_SEGMENT 0
#define GDT_RING0_CODE_SEGMENT 1
#define GDT_RING0_DATA_SEGMENT 2
#define GDT_RING3_DATA_SEGMENT 3
#define GDT_RING3_CODE_SEGMENT 4
#define GDT_TSS_SEGMENT 5

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

struct tss_entry {
	uint32_t reserved0;
	uint64_t rsp0;
	uint64_t rsp1;
	uint64_t rsp2;
	uint64_t reserved1;
	uint64_t interrupt_stack_table1;
	uint64_t interrupt_stack_table2;
	uint64_t interrupt_stack_table3;
	uint64_t interrupt_stack_table4;
	uint64_t interrupt_stack_table5;
	uint64_t interrupt_stack_table6;
	uint64_t interrupt_stack_table7;
	uint64_t reserved2;
	uint16_t reserved3;
	uint16_t iopb_offset;
} __attribute__((packed));

// We use +1 cause the tss descriptor is twice as big
static struct gdt_entry g_gdt_entries[GDT_ENTRIES + 1];
static struct gdt_desc g_gdt_descriptor;
static uint8_t g_tss_ring0_stack[KERNEL_STACK_SIZE];
static struct tss_entry g_tss_entry;

static void setup_tss_entry();

extern void gdt_reload_segments(uint16_t code_sel, uint16_t data_sel);
void init_gdt() {
	memset(&g_gdt_entries[GDT_NULL_SEGMENT], 0, sizeof(struct gdt_entry));

	g_gdt_entries[GDT_RING0_CODE_SEGMENT].limit = 0;
	g_gdt_entries[GDT_RING0_CODE_SEGMENT].base_low = 0;
	g_gdt_entries[GDT_RING0_CODE_SEGMENT].base_mid = 0;
	g_gdt_entries[GDT_RING0_CODE_SEGMENT].access = 0x9A;
	g_gdt_entries[GDT_RING0_CODE_SEGMENT].granularity = 0x20;
	g_gdt_entries[GDT_RING0_CODE_SEGMENT].base_high = 0;

	g_gdt_entries[GDT_RING0_DATA_SEGMENT].limit = 0;
	g_gdt_entries[GDT_RING0_DATA_SEGMENT].base_low = 0;
	g_gdt_entries[GDT_RING0_DATA_SEGMENT].base_mid = 0;
	g_gdt_entries[GDT_RING0_DATA_SEGMENT].access = 0x92;
	g_gdt_entries[GDT_RING0_DATA_SEGMENT].granularity = 0x00;
	g_gdt_entries[GDT_RING0_DATA_SEGMENT].base_high = 0;

	g_gdt_entries[GDT_RING3_DATA_SEGMENT].limit = 0;
	g_gdt_entries[GDT_RING3_DATA_SEGMENT].base_low = 0;
	g_gdt_entries[GDT_RING3_DATA_SEGMENT].base_mid = 0;
	g_gdt_entries[GDT_RING3_DATA_SEGMENT].access = 0xF2;
	g_gdt_entries[GDT_RING3_DATA_SEGMENT].granularity = 0x00;
	g_gdt_entries[GDT_RING3_DATA_SEGMENT].base_high = 0;

	g_gdt_entries[GDT_RING3_CODE_SEGMENT].limit = 0;
	g_gdt_entries[GDT_RING3_CODE_SEGMENT].base_low = 0;
	g_gdt_entries[GDT_RING3_CODE_SEGMENT].base_mid = 0;
	g_gdt_entries[GDT_RING3_CODE_SEGMENT].access = 0xFA;
	g_gdt_entries[GDT_RING3_CODE_SEGMENT].granularity = 0x20;
	g_gdt_entries[GDT_RING3_CODE_SEGMENT].base_high = 0;

	g_gdt_entries[GDT_TSS_SEGMENT].limit = sizeof(struct tss_entry) - 1;
	g_gdt_entries[GDT_TSS_SEGMENT].base_low = (uint16_t)((size_t)&g_tss_entry & 0xFFFF);
	g_gdt_entries[GDT_TSS_SEGMENT].base_mid = (uint8_t)(((size_t)&g_tss_entry >> 16) & 0xFF);
	g_gdt_entries[GDT_TSS_SEGMENT].base_high = (uint8_t)(((size_t)&g_tss_entry >> 24) & 0xFF);
	g_gdt_entries[GDT_TSS_SEGMENT].access = 0x89;
	g_gdt_entries[GDT_TSS_SEGMENT].granularity = 0x0;

	*((uint64_t*)(&g_gdt_entries[GDT_TSS_SEGMENT + 1])) = (size_t)&g_tss_entry >> 32;
	setup_tss_entry();

	g_gdt_descriptor =
		(struct gdt_desc){.limit = (sizeof(g_gdt_entries) - 1), .base = (size_t)&g_gdt_entries};

	asm volatile("lgdt %0" : : "m"(g_gdt_descriptor));

	uint16_t code_sel = GDT_RING0_CODE_SEGMENT << 3;
	uint16_t data_sel = GDT_RING0_DATA_SEGMENT << 3;
	gdt_reload_segments(code_sel, data_sel);

	asm volatile("ltr %0" : : "r"((uint16_t)(GDT_TSS_SEGMENT << 3)));

	printk("Initialized GDT\n");
}

uint8_t get_segment_index(enum gdt_segments segment) {
	switch (segment) {
		case GS_CODE_SEGMENT:
			return GDT_RING0_CODE_SEGMENT;
		default:
			KERNEL_PANIC("Invalid GDT segment");
	}
}

static void setup_tss_entry() {
	memset(&g_tss_entry, 0, sizeof(struct tss_entry));
	g_tss_entry.rsp0 = (uint64_t)(g_tss_ring0_stack + KERNEL_STACK_SIZE);
	g_tss_entry.iopb_offset = sizeof(struct tss_entry);
}
