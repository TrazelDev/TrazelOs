#pragma once

#include <include/types.h>
#include <kernel/include/intrrupts.h>

enum legacy_isa_irq;

void init_ioapic();
void init_lapic();
void apic_send_eoi();
void apic_set_legacy_irq_desc_num(enum legacy_isa_irq irq, uint32_t desc_num);
void apic_setup_timer_handler(void (*handler)(struct interrupt_info* state));

enum legacy_isa_irq {
	LIRQ_PIT = 0,
	LIRQ_KEYBOARD,
	LIRQ_CASCADE,
	LIRQ_COM2,
	LIRQ_COM1,
	LIRQ_LPT2,
	LIRQ_FLOPPY_DISK,
	LIRQ_LPT1,
	LIRQ_CMOS_RTC,
	LIRQ_FREE_FOR_PERIPHERALS1,
	LIRQ_FREE_FOR_PERIPHERALS2,
	LIRQ_FREE_FOR_PERIPHERALS3,
	LIRQ_PS2_MOUSE,
	LIRQ_FPU,
	LIRQ_PRIMARY_ATA_HARD_DISK,
	LIRQ_SECONDARY_ATA_HARD_DISK,
	LIRQ_COUNT,
};
