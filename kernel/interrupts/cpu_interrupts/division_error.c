#include "division_error.h"
#include "kernel/interrupts/cpu_interrupts/cpu_exceptions_def.h"
#include "kernel/interrupts/idt.h"
#include "kernel/interrupts/idt_defs.h"
#include "kernel/printk/printk.h"

void c_isr_division_error();
void init_diviosn() {
	static struct idt_entry division_error_entry;
	extern uint64_t asm_isr_division_error;
	create_interrupt_desc(&division_error_entry, (uint64_t)&asm_isr_division_error,
						  INTTYP_TRAP_GATE, 0, 0);
	load_new_interrupt(CEI_DIVIDE_ERROR, &division_error_entry);
}

void c_isr_division_error() {
	printk("You have been a naughty boy huh, dividing by zero\n");
	while (true) {
		asm volatile("hlt");
	}
}
