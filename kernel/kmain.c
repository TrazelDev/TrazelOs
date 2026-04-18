#include <drivers/char_device.h>
#include <drivers/ps2_keyboard.h>
#include <drivers/vga_text.h>
#include <include/io.h>
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
#include <kernel/include/scheduler.h>
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

void func1() {
	static uint64_t count = 0;
	while (true) {
		count++;
		printk("A %d\n", count);
	}
}
void func2() {
	static uint64_t count = 0;
	while (true) {
		count++;
		printk("B %d\n", count);
	}
}

void func3() {
	static uint64_t count = 0;
	while (true) {
		count++;
		printk("C %d\n", count);
	}
}
void func4() {
	static uint64_t count = 0;
	while (true) {
		count++;
		printk("D %d\n", count);
	}
}

int kmain() {
	init_printk();

	init_gdt();

	init_cpu_exceptions();

	init_pmm(memmap_request.response, hhdm_request.response);
	init_vmm(hhdm_request.response);
	init_kernel_heap();

	init_msr_cpu();

	init_hardware_interrupts();
	init_acpi(rsdp_request.response);
	init_madt();
	init_ioapic();
	init_lapic();

	struct char_device* ps2_keyboard = ps2_keyboard_init();

	init_scheduler();
	scheduler_add_task(func1);
	scheduler_add_task(func2);
	scheduler_add_task(func3);
	scheduler_add_task(func4);
	scheduler_handover_execution();

	// set_cpu_exception_handler(CEI_DIVIDE_ERROR, exception_handler);
	// set_cpu_exception_handler(CEI_PAGE_FAULT, exception_handler);

	while (true) {
		asm volatile("hlt");
	}
}
