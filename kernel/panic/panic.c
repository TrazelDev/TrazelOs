#include <kernel/include/printk.h>

#include "kernel/include/panic.h"

__attribute__((noreturn)) void kernel_panic(const char* msg, const char* file_name, size_t line) {
	asm volatile("cli");

	printk("\n\n\n=============================\n");
	printk("KERNEL PANIC!\n");
	printk("Error: %s\n", msg);
	printk("File: %s\n", file_name);
	printk("Line: %d\n", line);
	printk("=============================\n");

	while (true) {
		asm volatile("hlt");
	}
}
