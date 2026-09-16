#pragma once
#include <include/types.h>

/* This struct is based on the gdt.asm in bootloader more precise documentation is in there */
// struct gdt_entry {
// 	uint16_t limit;
// 	uint16_t base_low;
// 	uint8_t base_mid;
// 	uint8_t access;
// 	uint8_t granularity;
// 	uint8_t base_high;
// } __attribute__((packed));

/** More detailed documentation inside https://wiki.osdev.org/Global_Descriptor_Table */
struct gdt_entry {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_mid;
	struct {
		uint8_t access : 1;	 // CPU sets it to 1 when segment is accessed

		// For code segments: Readable bit. If clear (0), read access for this segment is not
		// allowed. If set (1) read access is allowed. Write access is never allowed for code
		// segments. For data segments: Writeable bit. If clear (0), write access for this segment
		// is not allowed. If set (1) write access is allowed. Read access is always allowed for
		// data segments.
		uint8_t read_write : 1;

		// For data selectors: Direction bit. If clear (0) the segment grows up. If set (1) the
		// segment grows down, ie. the Offset has to be greater than the Limit.
		// For code selectors:
		// Conforming bit. If clear (0) code in this segment can only be executed from the ring set
		// in DPL. If set (1) code in this segment can be executed from an equal or lower privilege
		// level. For example, code in ring 3 can far-jump to conforming code in a ring 2 segment.
		// The DPL field represent the highest privilege level that is allowed to execute the
		// segment. For example, code in ring 0 cannot far-jump to a conforming code segment where
		// DPL is 2, while code in ring 2 and 3 can. Note that the privilege level remains the same,
		// ie. a far-jump from ring 3 to a segment with a DPL of 2 remains in ring 3 after the jump.
		uint8_t direction_conforming : 1;
		uint8_t executable : 1;					 // 0 = data segment, 1 = code segment
		uint8_t descriptor_type : 1;			 // 0 = tss segment, 1 = code or data segment
		uint8_t descriptor_privilege_level : 2;	 // Ring level (0-3)
		uint8_t present : 1;					 // must always be 1 for a valid segment
	} access;
	uint8_t limit_high : 4;
	uint8_t reserved : 1;
	uint8_t long_mode : 1;	// 1 = 64-bit code segment, 0 = 32-bit code segment

	// if long_mode=1 => size_flag=0,
	// long_mode=0 for data segment usually set to 1 conventionally and for emulator stability
	uint8_t size_flag : 1;
	uint8_t granularity : 1;  // 1 - scales the limit by 4k, 0 - limit is as is
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
