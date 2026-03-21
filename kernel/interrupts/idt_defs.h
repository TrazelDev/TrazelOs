#pragma once
#include <include/types.h>
// Documentation can be found here: https://wiki.osdev.org/Interrupt_Descriptor_Table

enum interrupt_type;
union idt_selector;
struct idt_entry;
struct idt_register;
/* Takes a pointer to an empty interrupt descriptor and fills it with the given params */
void create_interrupt_desc(struct idt_entry* entry, uint64_t isr_addr, enum interrupt_type type,
						   uint8_t interrupt_stack_table, uint8_t ring_access_to_interrupt);

/**
 * IDT register (10 bytes).
 * Loaded via LIDT instruction to tell CPU where IDT is located.
 */
struct idt_register {
	uint16_t size;
	uint64_t idt_address;
} __attribute__((packed));

/* Tells us what segments to use for the interrupt, whether to use GDT or LDT and privelges
 * (Mostly obselste in x86_64 but required for backwards compatibility)
 * https://wiki.osdev.org/Segment_Selector
 */
union idt_selector {
	struct {
		/* Requestor Privilege Level (RPL):
		 * 00 - same as descriptor privilege
		 * 01 - higher privilege
		 * 10 - reserved
		 * 11 - reserved
		 */
		uint16_t privilege_level : 2;

		/* Table indicator: 0 = GDT, 1 = LDT */
		uint16_t table_indicator : 1;

		/* Segment index (0=null, 1=code, 2=data, ...) */
		uint16_t index : 13;
	} attr;
	uint16_t raw;
} __attribute__((packed));

#define MAX_IDT_ENTRIES_COUNT 256

/**
 * Interrupt gate types.
 * - INTERRUPT_GATE: Auto-disables interrupts (use for faults)
 * - TRAP_GATE: Doesn't disable interrupts (use for traps)
 * - TASK_GATE: Hardware task switch (rarely used)
 */
enum interrupt_type {
	INTTYP_INTERRUPT_GATE = 0xE,
	INTTYP_TRAP_GATE = 0xF,
	INTTYP_TASK_GATE = 0x5,
};
/* Explained in details in: https://wiki.osdev.org/Interrupt_Descriptor_Table */
struct idt_types_attributes {
	/* Interrupt type from the struct above */
	uint8_t interrupt_type : 4;
	/* Always 0 */
	uint8_t zero_bit : 1;
	/* DPL the ring level that is able to access the interrupt */
	uint8_t cpu_privilege_level : 2;
	/* Presenet bit always set to 1 */
	uint8_t present_bit : 1;
};

/**
 * 64-bit IDT entry (16 bytes total).
 * Describes an interrupt handler: address, privilege, and type.
 */
struct idt_entry {
	/* 2 low bytes of the isr */
	uint16_t offset_low;

	/* Selector for the entry explained in detail: https://wiki.osdev.org/Segment_Selector */
	union idt_selector selector;

	/* interrupt stack table index explained here in more detail:
	 * https://wiki.osdev.org/Task_State_Segment
	 */
	uint8_t interrupt_stack_table;

	struct idt_types_attributes types_attr;

	/* 6 top bytes of the isr */
	uint16_t offset_mid;
	uint32_t offset_high;

	/* Extra bits that are used for alignment ( in the future may they can be used for flags )
	 * Should be 0 always for now.
	 */
	uint32_t extra_bits;
} __attribute__((packed));
