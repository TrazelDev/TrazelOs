#include <kernel/include/cpuid.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "kernel/include/msr.h"

static bool g_msr_enabled = false;

void init_msr_cpu() {
	if (!cpuid_is_msr_supported()) {
		KERNEL_PANIC("This cpu does not support MSR");
	}

	g_msr_enabled = true;
	printk("Initialized msr Success\n");
}

uint64_t msr_get_register(uint32_t msr) {
	if (!g_msr_enabled) {
		KERNEL_PANIC("MSR is not enabled");
	}

	uint32_t low;
	uint32_t high;

	asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));

	return (((uint64_t)high << 32) | low);
}

void msr_set_register(uint64_t msr, uint64_t value) {
	if (!g_msr_enabled) {
		KERNEL_PANIC("MSR is not enabled");
	}

	uint32_t low = (uint32_t)(value & 0xFFFFFFFF);
	uint64_t high = (uint32_t)(value >> 32);

	asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(msr));
}
