#include <kernel/include/gdt.h>
#include <kernel/include/intrrupts.h>
#include <kernel/include/msr.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "kernel/include/syscall.h"

extern void asm_kernel_syscall_entrypoint();
extern void asm_jump_usermode(uint64_t usermode_entrypoint);

static void enable_system_call_extension();
static void setup_star_registers(uint64_t kernel_entry_addr);
static void setup_rflags_registers();
static void setup_gs_kernel_gs_base();

#define KERNEL_STACK_SIZE 4096
static uint8_t g_kernel_stack[KERNEL_STACK_SIZE];
// This has 2 elements cause the kernel stack is at the first and the second element is used in asm
uint8_t* g_kernel_ptr[2];

void init_usermode() {
	enable_system_call_extension();
	setup_star_registers((uint64_t)asm_kernel_syscall_entrypoint);
	setup_rflags_registers();
	setup_gs_kernel_gs_base();

	printk("Initializing ring3\n");
}
void usermode_jump_ring3(void (*init)()) { asm_jump_usermode((uint64_t)init); }

/** Function reuses the interrupt_info for the sake of universal API though some fields useless.
 * this function is the entry point for syscalls into the kernel.
 */
void syscall_kernel_handler(struct interrupt_info* syscall_info) {
	printk("YO I have just been to usermode I am so fucking cool, NO fucking way\n");
}

// module private functions:
// -------------------------------------------------------------------------------------------------

static void enable_system_call_extension() {
	union EFER_msr_register efer;
	efer.raw = msr_get_register(MR_EFER_MSR);
	efer.reg.sce = 1;  // enable the SCE (system call extension ) bit in EFER register
	msr_set_register(MR_EFER_MSR, efer.raw);
}

static void setup_star_registers(uint64_t kernel_entry_addr) {
	uint16_t ring0_cs_index = gdt_get_segment_index(GS_RING0_CODE_SEGMENT);
	uint16_t ring3_ss_index = gdt_get_segment_index(GS_RING3_DATA_SEGMENT);

	KERNEL_ASSERT(gdt_get_segment_index(GS_RING0_CODE_SEGMENT) + 1 ==
					  gdt_get_segment_index(GS_RING0_DATA_SEGMENT),
				  "for ring0 gdt segments: ss_index in gdt must be equal to cs_index+1")
	KERNEL_ASSERT(gdt_get_segment_index(GS_RING3_DATA_SEGMENT) + 1 ==
					  gdt_get_segment_index(GS_RING3_CODE_SEGMENT),
				  "for ring3 gdt segments: cs_index in gdt must be equal to ss_index+1")

	// cs = index , ss = index + 8:
	union gdt_segment_selector syscall_cs_ss = {
		.selector = {.privilege_level = 0, .table_indicator = 0, .index = ring0_cs_index}};
	// ss = index + 8, cs = index + 16:
	union gdt_segment_selector sysret_cs_ss = {
		.selector = {.privilege_level = 3, .table_indicator = 0, .index = ring3_ss_index - 1}};

	union STAR_msr_register star;
	star.raw = 0;
	star.reg.syscall_cs_ss_segment = syscall_cs_ss.raw;
	star.reg.systet_cs_ss_segment = sysret_cs_ss.raw;

	msr_set_register(MR_STAR_MSR, star.raw);
	msr_set_register(MR_LSTAR_MSR, kernel_entry_addr);
}

static void setup_rflags_registers() {
	// as a default setting up all the fields as zero
	union rflags_register fsmask = {.raw = 0};

	// All the turned on flags here mean they are cleared the second syscall occurs:
	fsmask.flags.interrupt_enable_flag = 1;	 // clearing the interrupts when syscall occurs
	fsmask.flags.direction_flag = 1;		 // related to the direction of string operations

	msr_set_register(MR_FSMASK_MSR, fsmask.raw);
}

static void setup_gs_kernel_gs_base() {
	// uint64_t kernel_gs = msr_get_register(MR_KERNEL_GS_BAS_MSR);
	// uint64_t regular_gs = msr_get_register(MR_GS_BAS_MSR);

	// printk("regular gs: %d, kernel gs: %d\n", regular_gs, kernel_gs);
	// msr_set_register(MR_KERNEL_GS_BAS_MSR, 0x1000);

	// kernel_gs = msr_get_register(MR_KERNEL_GS_BAS_MSR);
	// regular_gs = msr_get_register(MR_GS_BAS_MSR);
	// printk("regular gs: %d, kernel gs: %d\n", regular_gs, kernel_gs);

	g_kernel_ptr[0] = g_kernel_stack + KERNEL_STACK_SIZE;
	msr_set_register(MR_KERNEL_GS_BAS_MSR, (uint64_t)&g_kernel_ptr);
}
