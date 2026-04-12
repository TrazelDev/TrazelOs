// NOTE: This is not the real kernel but one for testing purposes in the refactoring of the
// bootloader The real kernel is src/kernel
#include <drivers/vga_text.h>
#include <include/types.h>
#include <include/vendor/limine.h>
#include <kernel/include/acpi.h>
#include <kernel/include/gdt.h>
#include <kernel/include/heap.h>
#include <kernel/include/intrrupts.h>
#include <kernel/include/madt.h>
#include <kernel/include/msr.h>
#include <kernel/include/panic.h>
#include <kernel/include/pmm.h>
#include <kernel/include/printk.h>
#include <kernel/include/vmm.h>

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
	used, section(".limine_requests"))) static volatile struct limine_rsdp_request rsdp_request = {
	.id = LIMINE_RSDP_REQUEST_ID, .revision = 0, .response = NULL};

__attribute__((
	used, section(".limine_requests_end"))) static volatile uint64_t limine_requests_end_marker[] =
	LIMINE_REQUESTS_END_MARKER;

void exception_handler(struct interrupt_info* info) {
	printk("It works but it should just crush in that case\n");
	while (true) {
		asm volatile("hlt");
	}
}

int kmain() {
	init_printk();

	init_gdt();

	init_cpu_exceptions();
	// set_cpu_exception_handler(CEI_DIVIDE_ERROR, exception_handler);
	// set_cpu_exception_handler(CEI_PAGE_FAULT, exception_handler);

	init_pmm(memmap_request.response, hhdm_request.response);
	init_vmm(hhdm_request.response);
	init_kernel_heap();

	init_msr_cpu();

	init_acpi(rsdp_request.response);
	init_madt();

	while (true) {
		asm volatile("hlt");
	}
}
