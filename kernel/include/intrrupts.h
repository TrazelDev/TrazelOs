#pragma once
#include <include/types.h>

enum cpu_exceptions_indexes;
struct exception_info;

/* @brief initializing the idt */
void init_cpu_exceptions();

void set_cpu_exception_handler(enum cpu_exceptions_indexes index,
							   void (*handler)(struct exception_info* info));

#define CPU_EXCEPTION_INTERRUPT_COUNT 32
enum cpu_exceptions_indexes {
	CEI_DIVIDE_ERROR = 0x0,
	CEI_DEBUG_EXCEPTION = 0x1,
	CEI_NMI_INTERRUPT = 0x2,
	CEI_BREAKPOINT = 0x3,
	CEI_OVERFLOW = 0x4,
	CEI_BOUND_RANGE_EXCEEDED = 0x5,
	CEI_INVALID_OPCODE = 0x6,
	CEI_DEVICE_NOT_AVAILABLE = 0x7,
	CEI_DOUBLE_FAULT = 0x8,
	CEI_COPROCESSOR_SEGMENT_OVERRUN = 0x9,
	CEI_INVALID_TSS = 0xA,
	CEI_SEGMENT_NOT_PRESENT = 0xB,
	CEI_STACK_SEGMENT_FAULT = 0xC,
	CEI_GENERAL_PROTECTION = 0xD,
	CEI_PAGE_FAULT = 0xE,
	CEI_X87_FLOATING_POINT_ERROR = 0x10,
	CEI_ALIGNMENT_CHECK = 0x11,
	CEI_MACHINE_CHECK = 0x12,
	CEI_SIMD_FLOATING_POINT_EXCEPTION = 0x13,
	CEI_VIRTUALIZATION_EXCEPTION = 0x14,
	CEI_CONTROL_PROTECTION_EXCEPTION = 0x15,
};

struct exception_info {
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rbp;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;

	/* The exception number of the interrupt that was triggered */
	uint64_t interrupt_index;
	/* If an interrupt contains an error code then it will be in this value
	 * Otherwise the value will be 0
	 * What contains the code or not will be in the table in the bottom of this page:
	 * https://wiki.osdev.org/Interrupt_Descriptor_Table
	 */
	uint64_t potential_error_code;
	/* the rip that the exception was triggered on */
	uint64_t exception_trigger_rip;
	/* the code segment padded to 8 bytes */
	uint64_t code_segment;
	uint64_t rflags;
	/* Original value of rsp (It is relevant cause stack can change when interrupt is triggered) */
	uint64_t original_rsp;
} __attribute__((packed));
