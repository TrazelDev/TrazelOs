#include <drivers/char_device.h>
#include <drivers/framebuffer_print.h>
#include <drivers/ps2_keyboard.h>
#include <drivers/vga_text.h>
#include <include/io.h>
#include <include/types.h>
#include <include/vendor/limine.h>
#include <kernel/include/acpi.h>
#include <kernel/include/apic.h>
#include <kernel/include/cmos.h>
#include <kernel/include/gdt.h>
#include <kernel/include/heap.h>
#include <kernel/include/intrrupts.h>
#include <kernel/include/madt.h>
#include <kernel/include/msr.h>
#include <kernel/include/panic.h>
#include <kernel/include/pmm.h>
#include <kernel/include/printk.h>
#include <kernel/include/process_manager.h>
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

void enable_sse(void) {
	uint64_t cr0, cr4;

	// 1. Read CR0, clear EM (bit 2), set MP (bit 1), and write it back
	asm volatile("movq %%cr0, %0" : "=r"(cr0));
	cr0 &= ~(1ULL << 2);  // Clear EM (Emulation)
	cr0 |= (1ULL << 1);	  // Set MP (Monitor Coprocessor)
	asm volatile("movq %0, %%cr0" : : "r"(cr0) : "memory");

	// 2. Read CR4, set OSFXSR (bit 9) and OSXMMEXCPT (bit 10), and write it back
	asm volatile("movq %%cr4, %0" : "=r"(cr4));
	cr4 |= (1ULL << 9);	  // Set OSFXSR (Fast FXSAVE/FXRSTOR)
	cr4 |= (1ULL << 10);  // Set OSXMMEXCPT (Unmasked SSE exceptions)
	asm volatile("movq %0, %%cr4" : : "r"(cr4) : "memory");
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
	init_cmos();

	vfs_init();
	init_usermode();
	enable_sse();
	init_process_manager();
	KERNEL_PANIC("Kernel failed to jump to ring3 init process");

	// This for installing the doom image:
	// curl -O https://www.jbserver.com/downloads/games/doom/misc/shareware/doom1.wad.zip
	// unzip doom1.wad.zip
	// mv DOOM1.WAD doom1.wad

	// TODO:
	// * Make the get time from startup in milseconds a proper syscall
	// * Separate your ps2 keyboard driver into only getting keycodes
	// * Create an actual tty driver
	// * Create an interface to actually get the framebuffer and a machnism that saves all of the
	// that before handing over control to the framebuffer or something like that.
	// Do not forget that when trying to release the framebuffer which is memory mapped IO you call
	// the pmm which will try to access the bitmap at a very high indexes therefor a kernel assert
	// you need to add logic to your pmm to not page release the memory mapped io
	// * Add the doom as one of the games you can play in your operating system
	// * Instruction how to cross compile busybox: https://www.busybox.net/FAQ.html#build
	// * Start using image chaining for docker cause it starts becoming a pain to have it all
	// managed in one file or read about things to manage the docke thing better
	// * Create stack page guards for the loaded processes
	// * Use -Wextra flag and test different optimization levels like O1 or O2
	// * See if you need to use goto for clean up instead of code duplications
}
