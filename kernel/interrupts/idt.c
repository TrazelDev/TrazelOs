#include <kernel/panic/panic.h>
#include <kernel/printk/printk.h>

#include "idt.h"
#include "idt_defs.h"

/** @brief The lidt instruction.
 * the function loads the interrupt register to memory which loads the interrupt descriptor
 * table to memory. this function all also sets the interrupt flag which enables interrupts to get
 * called in the os. */
static void load_idt_register();
/** the cli asm instruction */
static void cli();
/** the sti asm instruction */
static void sti();

static struct idt_entry g_interrupt_table[MAX_IDT_ENTRIES_COUNT];
static struct idt_register g_idt_register;

void init_cpu_exceptions() {
	g_idt_register.size = (uint16_t)(sizeof(g_interrupt_table));
	g_idt_register.idt_address = (uint64_t)(g_interrupt_table);
	load_idt_register();
}

static void load_idt_register() {
	// m - means get the address of the variable
	asm volatile("lidt %0" : : "m"(g_idt_register));
}

static void cli() { asm volatile("cli"); }
static void sti() { asm volatile("sti"); }

size_t load_new_interrupt(uint8_t index, struct idt_entry* desc) {
	KERNEL_ASSERT(desc != NULL);
	KERNEL_ASSERT(index < MAX_IDT_ENTRIES_COUNT);

	cli();
	g_interrupt_table[index] = *desc;
	// sti();

	return index;
}
