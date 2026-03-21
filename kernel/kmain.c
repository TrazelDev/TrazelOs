// NOTE: This is not the real kernel but one for testing purposes in the refactoring of the
// bootloader The real kernel is src/kernel
#include <drivers/vga_text.h>
#include <include/types.h>

#include "kernel/interrupts/cpu_interrupts/division_error.h"
#include "kernel/interrupts/idt.h"
#include "kernel/interrupts/idt_defs.h"
#include "kernel/panic/panic.h"
#include "kernel/printk/printk.h"

int kmain() {
	init_printk();
	init_cpu_exceptions();
	init_diviosn();

	struct idt_register check;
	asm volatile("sidt %0" : "=m"(check));
	printk("After LIDT: size=0x%x, addr=0x%x\n", check.size, check.idt_address);
	int a = 5 / 0;
	while (true) {
		asm volatile("hlt");
	}
}
