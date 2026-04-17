#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "kernel/include/intrrupts.h"

static void (*g_hardware_interrupt_handlers[HARDWARE_INTERRUPT_COUNT])(
	struct interrupt_info* info) = {NULL};

void hardware_interrupts_isr_central_handler(struct interrupt_info* info) {
	if (g_hardware_interrupt_handlers[info->interrupt_index - CPU_EXCEPTION_INTERRUPT_COUNT]) {
		g_hardware_interrupt_handlers[info->interrupt_index - CPU_EXCEPTION_INTERRUPT_COUNT](info);
		return;
	}

	printk("\n\n\nInterrupt number %d fired", info->interrupt_index);
	KERNEL_PANIC("Unexpected interrupt occurred");
}

void set_hardware_interrupt_handler(uint8_t index, void (*handler)(struct interrupt_info* info)) {
	KERNEL_ASSERT(index >= CPU_EXCEPTION_INTERRUPT_COUNT, "Invalid hardware interrupt index");
	g_hardware_interrupt_handlers[index - CPU_EXCEPTION_INTERRUPT_COUNT] = handler;
}

uint8_t get_free_interrupt_desc(void) {
	for (uint8_t i = 0; i < HARDWARE_INTERRUPT_COUNT; i++) {
		if (g_hardware_interrupt_handlers[i] == NULL) {
			return i + CPU_EXCEPTION_INTERRUPT_COUNT;
		}
	}

	KERNEL_PANIC(
		"No free hardware interrupt descriptor available, Please make sure there is a free one "
		"before calling this function");
}
