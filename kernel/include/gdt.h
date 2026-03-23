#pragma once
#include <include/types.h>

enum gdt_segments {
	GS_CODE_SEGMENT,
};

void init_gdt();
uint8_t get_segment_index(enum gdt_segments segment);
