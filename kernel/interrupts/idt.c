#include <include/types.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "idt_defs.h"
#include "kernel/include/intrrupts.h"

/** @brief The lidt instruction.
 * the function loads the interrupt register to memory which loads the interrupt descriptor
 * table to memory. this function all also sets the interrupt flag which enables interrupts to get
 * called in the os. */
static void load_idt_register();
/** the cli asm instruction */
static void cli();
/** the sti asm instruction */
static void sti();
/* @brief load a new interrupt descriptor to the idt
 * @param index the index of the interrupt in the idt
 * @param desc the interrupt descriptor to be loaded
 * @return index on success, other on failure
 */
static size_t load_new_interrupt(uint8_t index, struct idt_entry* desc);

static struct idt_entry g_interrupt_table[MAX_IDT_ENTRIES_COUNT];
static struct idt_register g_idt_register;

void init_cpu_exceptions() {
	g_idt_register.size = (uint16_t)(sizeof(g_interrupt_table));
	g_idt_register.idt_address = (uint64_t)(g_interrupt_table);

	extern uint64_t isr_stub_table[CPU_EXCEPTION_INTERRUPT_COUNT];
	struct idt_entry entry;
	for (uint8_t i = 0; i < CPU_EXCEPTION_INTERRUPT_COUNT; i++) {
		create_interrupt_desc(&entry, (uint64_t)isr_stub_table[i], INTTYP_TRAP_GATE, 0, 0);
		load_new_interrupt(i, &entry);
	}

	load_idt_register();
	printk("Initialized CPU exceptions\n");
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
