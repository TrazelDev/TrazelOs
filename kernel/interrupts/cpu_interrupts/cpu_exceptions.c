#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "kernel/include/intrrupts.h"

static void (*s_cpu_exception_handlers[CPU_EXCEPTION_INTERRUPT_COUNT])(
	struct interrupt_info* info) = {NULL};

void set_cpu_exception_handler(enum cpu_exceptions_indexes index,
							   void (*handler)(struct interrupt_info* info)) {
	KERNEL_ASSERT(index >= 0 && index < CPU_EXCEPTION_INTERRUPT_COUNT,
				  "Invalid CPU exception index");
	s_cpu_exception_handlers[index] = handler;
}

void cpu_exceptions_isr_central_handler(struct interrupt_info* info) {
	if (s_cpu_exception_handlers[info->interrupt_index]) {
		s_cpu_exception_handlers[info->interrupt_index](info);
		return;
	}

	printk("\n=== CPU Exception ===\n");
	printk("Exception #%d at RIP: %x\n", info->interrupt_index, info->rip);
	printk("RSP: %x | RFLAGS: %x\n", info->original_rsp, info->rflags);
	printk("Error Code: %x | Code Segment: %x", info->potential_error_code, info->code_segment);

	printk("\n=== General Purpose Registers ===\n");
	printk("RAX: %x | RBX: %x | RCX: %x\n", info->rax, info->rbx, info->rcx);
	printk("RDX: %x | RSI: %x | RDI: %x\n", info->rdx, info->rsi, info->rdi);
	printk("RBP: %x", info->rbp);

	printk("\n=== Extended Registers ===\n");
	printk("R8:  %x | R9:  %x | R10: %x\n", info->r8, info->r9, info->r10);
	printk("R11: %x | R12: %x | R13: %x\n", info->r11, info->r12, info->r13);
	printk("R14: %x | R15: %x", info->r14, info->r15);
	printk("\n==========================\n");
	KERNEL_PANIC("Unexpected CPU exception occurred");

	while (true) {
		asm volatile("hlt");
	}
}
