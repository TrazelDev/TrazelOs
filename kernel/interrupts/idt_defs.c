#include <kernel/panic/panic.h>

#include "idt_defs.h"

/* the function that is setting the selector in the current way we want it to be
 * configured: gdt, the same privilege level as the descriptor table and in the code segment */
static void set_selector(union idt_selector* selector);
/* gets the isr offset from the descriptor */
static uint64_t get_offset(struct idt_entry* descriptor);
/* sets the isr offset into the descriptor */
static void set_offset(struct idt_entry* descriptor, uint64_t offset);

#define MAX_INTERRUPT_STACK_TABLES 7
void create_interrupt_desc(struct idt_entry* entry, uint64_t isr_addr, enum interrupt_type type,
						   uint8_t interrupt_stack_table, uint8_t ring_access_to_interrupt) {
	KERNEL_ASSERT(interrupt_stack_table < 7);

	set_offset(entry, isr_addr);
	set_selector(&entry->selector);
	entry->interrupt_stack_table = interrupt_stack_table;
	entry->extra_bits = 0;

	// Setting up the types and attributes:
	entry->types_attr.cpu_privilege_level = ring_access_to_interrupt;
	entry->types_attr.present_bit = 1;
	entry->types_attr.interrupt_type = type;
	entry->types_attr.zero_bit = 0;
}

#define GDT_INDICATOR 0
#define SAME_PRIVILEGE_AS_DESCRIPTOR_TABLE 0
#define CODE_SEGMENT 1
void set_selector(union idt_selector* selector) {
	selector->attr.table_indicator = GDT_INDICATOR;
	selector->attr.privilege_level = SAME_PRIVILEGE_AS_DESCRIPTOR_TABLE;
	selector->attr.index = CODE_SEGMENT;
}

// Extracting the offset:
#define SET_OFFSET_LOW_BIT_MASK(offset) (uint16_t)(((offset) & 0x000000000000FFFF))
#define SET_OFFSET_MID_BIT_MASK(offset) (uint16_t)(((offset) & 0x00000000FFFF0000) >> 16)
#define SET_OFFSET_HIGH_BIT_MASK(offset) (uint32_t)(((offset) & 0xFFFFFFFF00000000) >> 32)
#define GET_OFFSET_LOW_BIT_MASK(desc) ((uint64_t)(desc)->offset_low)
#define GET_OFFSET_MID_BIT_MASK(desc) ((uint64_t)(desc)->offset_mid) << 16
#define GET_OFFSET_HIGH_BIT_MASK(desc) ((uint64_t)(desc)->offset_high) << 32

static void set_offset(struct idt_entry* descriptor, uint64_t offset) {
	descriptor->offset_low = SET_OFFSET_LOW_BIT_MASK(offset);
	descriptor->offset_mid = SET_OFFSET_MID_BIT_MASK(offset);
	descriptor->offset_high = SET_OFFSET_HIGH_BIT_MASK(offset);
}

static uint64_t get_offset(struct idt_entry* descriptor) {
	uint64_t offset = 0;

	// using bitwise or in order to set the value offset with the right bits:
	offset |= GET_OFFSET_LOW_BIT_MASK(descriptor);
	offset |= GET_OFFSET_MID_BIT_MASK(descriptor);
	offset |= GET_OFFSET_HIGH_BIT_MASK(descriptor);

	return offset;
}
