#pragma once
#include <include/types.h>

enum gdt_segments {
	GS_RING0_CODE_SEGMENT,
	GS_RING3_CODE_SEGMENT,
	GS_RING0_DATA_SEGMENT,
	GS_RING3_DATA_SEGMENT,
};

union gdt_segment_selector {
	struct {
		/* Requestor Privilege Level (RPL):
		 * 00 - ring 0
		 * 01 - ring 1
		 * 10 - ring 2
		 * 11 - ring 3
		 */
		uint16_t privilege_level : 2;

		/* Table indicator: 0 = GDT, 1 = LDT */
		uint16_t table_indicator : 1;

		/* index into the GDT or LDT */
		uint16_t index : 13;
	} selector;

	uint16_t raw;
} __attribute__((packed));

void init_gdt();
uint16_t gdt_get_segment_index(enum gdt_segments segment);
