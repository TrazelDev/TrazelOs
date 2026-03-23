// NOTE: This is not the real kernel but one for testing purposes in the refactoring of the
// bootloader The real kernel is src/kernel
#include <drivers/vga_text.h>
#include <include/types.h>
#include <kernel/include/gdt.h>
#include <kernel/include/intrrupts.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

void exception_handler(struct exception_info* info) {
	printk("It works but it should just crush in that case\n");
	while (true) {
		asm volatile("hlt");
	}
}

int kmain() {
	init_printk();
	init_gdt();
	init_cpu_exceptions();

	set_cpu_exception_handler(CEI_DIVIDE_ERROR, exception_handler);
	set_cpu_exception_handler(CEI_PAGE_FAULT, exception_handler);
	// struct idt_register check;
	// asm volatile("sidt %0" : "=m"(check));
	// printk("After LIDT: size=0x%x, addr=0x%x\n", check.size, check.idt_address);

	int a = 5 / 0;
	// volatile int64_t a = -9223372036854775808;
	// volatile int64_t b = -1;
	// volatile int64_t c = a / b;
	char* val = (char*)0x300000;
	val[0] = 'a';

	while (true) {
		asm volatile("hlt");
	}
}
