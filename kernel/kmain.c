#include <drivers/char_device.h>
#include <drivers/framebuffer_print.h>
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
#include <kernel/include/syscall.h>
#include <kernel/include/vfs.h>
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

__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request
	framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST_ID, .revision = 0};

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
	init_printk(framebuffer_request.response);
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

	// scheduling:
	init_scheduler();
	// scheduler_add_task(func1);
	// scheduler_add_task(func2);
	// scheduler_add_task(func3);
	// scheduler_add_task(func4);
	// scheduler_handover_execution();

	// setting up cpu exectption
	// set_cpu_exception_handler(CEI_DIVIDE_ERROR, exception_handler);
	// set_cpu_exception_handler(CEI_PAGE_FAULT, exception_handler);

	vfs_init();

	// jumping to user mode:
	void* ptr = pmm_alloc_page();
	vmm_map_page(vmm_get_curr_pagemap(), (void*)0x400000, ptr,
				 MPF_WRITABLE_PAGE | MPF_USER_ACCESSIBLE);
	uint8_t* user_code = (uint8_t*)0x400000;
	// user_code[0] = 0xF4;  // hlt
	// user_code[0] = 0xF4;
	// syscall:
	user_code[0] = 0x0F;
	user_code[1] = 0x05;
	user_code[2] = 0xEB;  // jmp instruction
	user_code[3] = 0xFE;  // relative offset to self
	void (*init)() = (void (*)())0x400000;

	init_usermode();
	usermode_jump_ring3(init);

	while (true) {
		asm volatile("hlt");
	}
}
