// NOTE: This is not the real kernel but one for testing purposes in the refactoring of the
// bootloader The real kernel is src/kernel
#include <drivers/vga_text.h>
#include <include/types.h>
#include <include/vendor/limine.h>
#include <kernel/include/gdt.h>
#include <kernel/include/intrrupts.h>
#include <kernel/include/panic.h>
#include <kernel/include/pmm.h>
#include <kernel/include/printk.h>

__attribute__((
	used,
	section(".limine_requests_start"))) static volatile uint64_t limine_requests_start_marker[] =
	LIMINE_REQUESTS_START_MARKER;

__attribute__((
	used,
	section(".limine_requests"))) static volatile struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST_ID, .revision = 0, .response = NULL};

__attribute__((
	used, section(".limine_requests"))) static volatile struct limine_hhdm_request hhdm_request = {
	.id = LIMINE_HHDM_REQUEST_ID, .revision = 0, .response = NULL};

__attribute__((
	used, section(".limine_requests_end"))) static volatile uint64_t limine_requests_end_marker[] =
	LIMINE_REQUESTS_END_MARKER;

void exception_handler(struct exception_info* info) {
	printk("It works but it should just crush in that case\n");
	while (true) {
		asm volatile("hlt");
	}
}

void func(volatile struct limine_hhdm_response* response) { printk("%x", response->offset); }

int kmain() {
	init_printk();
	init_gdt();
	init_cpu_exceptions();
	init_pmm(memmap_request.response, hhdm_request.response);

	set_cpu_exception_handler(CEI_DIVIDE_ERROR, exception_handler);
	set_cpu_exception_handler(CEI_PAGE_FAULT, exception_handler);

	// struct idt_register check;
	// asm volatile("sidt %0" : "=m"(check));
	// printk("After LIDT: size=0x%x, addr=0x%x\n", check.size, check.idt_address);

	int a = 5 / 0;
	// volatile int64_t a = -9223372036854775808;
	// volatile int64_t b = -1;
	// volatile int64_t c = a / b;
	// char* val = (char*)0x3000000;
	// val[0] = 'a';
	// int val = 2147483647;
	// asm volatile("int $0x0E");

	// val = val + 1;
	while (true) {
		asm volatile("hlt");
	}
}
