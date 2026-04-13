// NOTE: This is not the real kernel but one for testing purposes in the refactoring of the
// bootloader The real kernel is src/kernel
#include <drivers/vga_text.h>
#include <include/types.h>
#include <include/vendor/limine.h>
#include <kernel/include/acpi.h>
#include <kernel/include/apic.h>
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

#include <include/io.h>
void exception_handler(struct interrupt_info* info) {
	uint8_t scan_code = inb(IO_KEYBOARD_DATA_PORT);

	printk("key: 0x%x\n", scan_code);
	apic_send_eoi();
}

int kmain() {
	init_printk();

	init_gdt();

	init_cpu_exceptions();

	init_pmm(memmap_request.response, hhdm_request.response);
	init_vmm(hhdm_request.response);
	init_kernel_heap();

	init_msr_cpu();
	init_acpi(rsdp_request.response);
	init_madt();
	init_apic();
	init_hardware_interrupts();

	// TODO:
	// 1. Add a way to get the actual free interrupt number and the use it.
	// 2. Refactor the madt and apic the code quality is not that high right now.
	// 3. commit all of the changes gradually for the work you have done.
	// 4. Create an actual keyboard driver and not just an interrupt handler for testing purposes.
	// testing:
	//
	// set_cpu_exception_handler(CEI_DIVIDE_ERROR, exception_handler);
	// set_cpu_exception_handler(CEI_PAGE_FAULT, exception_handler);
	apic_set_legacy_irq_desc_num(LIRQ_KEYBOARD, 35);
	set_hardware_interrupt_handler(35, exception_handler);

	while (true) {
		asm volatile("hlt");
	}
}
