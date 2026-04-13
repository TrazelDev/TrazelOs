#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "kernel/include/intrrupts.h"

static void (*g_hardware_interrupt_handlers[HARDWARE_INTERRUPT_COUNT])(
	struct interrupt_info* info) = {NULL};

void hardware_interrupts_isr_central_handler(struct interrupt_info* info) {
	if (g_hardware_interrupt_handlers[info->interrupt_index]) {
		g_hardware_interrupt_handlers[info->interrupt_index](info);
		return;
	}

	printk("\n\n\nInterrupt number %d fired", info->interrupt_index);
	KERNEL_PANIC("Unexpected interrupt occurred");
}

void set_hardware_interrupt_handler(uint8_t index, void (*handler)(struct interrupt_info* info)) {
	KERNEL_ASSERT(index >= CPU_EXCEPTION_INTERRUPT_COUNT && index < HARDWARE_INTERRUPT_COUNT,
				  "Invalid hardware interrupt index");
	g_hardware_interrupt_handlers[index] = handler;
}
