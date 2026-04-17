// NOTE: This is not full the kernel currently it is undergoing a big refactor but this version is
// the primary. More code is present in legacy_src/kernel

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

void pit_interrupt_handler() {
	static int count = 0;
	count++;
	printk("The count is %d\n", count);
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

	// set_cpu_exception_handler(CEI_DIVIDE_ERROR, exception_handler);
	// set_cpu_exception_handler(CEI_PAGE_FAULT, exception_handler);

	// uint8_t keyboard_desc = get_free_interrupt_desc();
	// printk("The keyboard desc is %d\n", keyboard_desc);
	// apic_set_legacy_irq_desc_num(LIRQ_KEYBOARD, keyboard_desc);
	// set_hardware_interrupt_handler(keyboard_desc, keyboard_interrupt_handler);

	// uint8_t pit_desc = get_free_interrupt_desc();
	// printk("The pit desc is %d\n", keyboard_desc);
	// apic_set_legacy_irq_desc_num(LIRQ_PIT, pit_desc);
	// set_hardware_interrupt_handler(pit_desc, pit_interrupt_handler);

	ps2_keyboard_init();

#define COM1_PORT 0x3f8
#define COM1_STATUS (COM1_PORT + 5)
#define COM1_DATA (COM1_PORT + 0)

	uint8_t received_val;
	while (true) {
		if (inb(COM1_STATUS) & 0x1) {
			received_val = inb(COM1_DATA);
			if (received_val == '\r') {
				outb(COM1_DATA, '\n');
			} else if (received_val == 127) {
				outb(COM1_DATA, 8);
				outb(COM1_DATA, ' ');
				outb(COM1_DATA, 8);
			} else {
				outb(COM1_DATA, received_val);
			}
		}
	}

	while (true) {
		asm volatile("hlt");
	}
}
