#include <include/io.h>
#include <kernel/include/cpuid.h>
#include <kernel/include/intrrupts.h>
#include <kernel/include/madt.h>
#include <kernel/include/msr.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>
#include <kernel/include/vmm.h>

#include "kernel/include/apic.h"
#include "pit.h"

/** https://wiki.osdev.org/APIC#Local_APIC_registers */
enum lapic_registers {
	LAPIC_REG_APICID = 0x20,
	LAPIC_REG_APICVER = 0x30,
	LAPIC_REG_TASKPRIOR = 0x80,
	LAPIC_REG_EOI = 0x0B0,
	LAPIC_REG_LDR = 0x0D0,
	LAPIC_REG_DFR = 0x0E0,
	LAPIC_REG_SPURIOUS = 0x0F0,
	LAPIC_REG_ESR = 0x280,
	LAPIC_REG_ICRL = 0x300,
	LAPIC_REG_ICRH = 0x310,
	LAPIC_REG_LVT_TMR = 0x320,
	LAPIC_REG_LVT_PERF = 0x340,
	LAPIC_REG_LVT_LINT0 = 0x350,
	LAPIC_REG_LVT_LINT1 = 0x360,
	LAPIC_REG_LVT_ERR = 0x370,
	LAPIC_REG_TMRINITCNT = 0x380,
	LAPIC_REG_TMRCURRCNT = 0x390,
	LAPIC_REG_TMRDIV = 0x3E0,
	LAPIC_REG_LAST = 0x38F,
};

#define APIC_LVT_INT_MASKED 0x10000
#define LAPIC_SW_ENABLE 0x100
#define LAPIC_CPUFOCUS 0x200
#define LAPIC_NMI (4 << 8)
#define LAPIC_TMR_PERIODIC 0x20000
#define LAPIC_TMR_BASEDIV (1 << 20)

#define GET_APIC_REDIRECTION_ENTRY_INDEX(index) (0x10 + ((index) * 2))
uint32_t volatile* g_io_apic_base = NULL;
uint32_t volatile* g_lapic_base = NULL;
void (*g_lapic_timer_handler)() = NULL;

union apic_redirection_entry {
	struct {
		uint64_t vector : 8;
		uint64_t delv_mode : 3;
		uint64_t dest_mode : 1;
		uint64_t delv_status : 1;
		uint64_t pin_polarity : 1;
		uint64_t remote_irr : 1;
		uint64_t trigger_mode : 1;
		uint64_t mask : 1;
		uint64_t reserved : 39;
		uint64_t destination : 8;
	} attributes;
	struct {
		uint32_t lower_dword;
		uint32_t upper_dword;
	} dwords;

	uint64_t raw;
};

static union apic_redirection_entry read_ioapic_redirection_entry(uint64_t index);
static void write_ioapic_redirection_entry(union apic_redirection_entry entry, uint64_t index);
static uint32_t read_lapic_register(enum lapic_registers reg_offset);
static void write_lapic_register(enum lapic_registers reg_offset, uint32_t value);
static void disable_pic();
static void print_apic_redirection_entry(union apic_redirection_entry entry);
static void lapic_timer_handler();

void init_ioapic() {
	if (!cpuid_is_apic_supported()) {
		KERNEL_PANIC("APIC is not supported on this CPU");
	}

	g_io_apic_base = vmm_phys_to_virt_hhdm((void*)madt_get_io_apic_base_addr());
	g_lapic_base = vmm_phys_to_virt_hhdm((void*)madt_get_lapic_base_addr());
	disable_pic();

	printk("Initialized apic\n");
}

void init_lapic() {
	const uint32_t MILLISECONDS_PER_TIMER_ACTIVATION = 1;

	// Tell APIC timer to use divider 16
	write_lapic_register(LAPIC_REG_TMRDIV, 0x3);

	// Set APIC init counter to -1
	write_lapic_register(LAPIC_REG_TMRINITCNT, 0xFFFFFFFF);

	// Perform PIT-supported sleep
	pit_spin_sleep_ms(MILLISECONDS_PER_TIMER_ACTIVATION);

	// Stop the APIC timer
	write_lapic_register(LAPIC_REG_LVT_TMR, APIC_LVT_INT_MASKED);

	// Setting up APIC timer handler to be called every MILLISECONDS_PER_TIMER_ACTIVATION:
	uint32_t ticks_per_activation = 0xFFFFFFFF - read_lapic_register(LAPIC_REG_TMRCURRCNT);
	uint8_t idt_index = get_free_interrupt_desc();
	set_hardware_interrupt_handler(idt_index, lapic_timer_handler);
	write_lapic_register(LAPIC_REG_LVT_TMR, idt_index | LAPIC_TMR_PERIODIC);
	write_lapic_register(LAPIC_REG_TMRDIV, 0x3);
	write_lapic_register(LAPIC_REG_TMRINITCNT, ticks_per_activation);
}

void apic_send_eoi() { write_lapic_register(LAPIC_REG_EOI, 0); }

void apic_set_legacy_irq_desc_num(enum legacy_isa_irq irq, uint32_t desc_num) {
	uint64_t pin_index = madt_get_gsi_for_irq(irq);

	union apic_redirection_entry entry = read_ioapic_redirection_entry(pin_index);
	entry.attributes.mask = 0;
	entry.attributes.destination = 0;
	entry.attributes.vector = desc_num;
	write_ioapic_redirection_entry(entry, pin_index);
}

void apic_setup_timer_handler(void (*handler)()) { g_lapic_timer_handler = handler; }

// module private functions:
// -------------------------------------------------------------------------------------------------

static union apic_redirection_entry read_ioapic_redirection_entry(uint64_t index) {
	union apic_redirection_entry entry;
	*g_io_apic_base = GET_APIC_REDIRECTION_ENTRY_INDEX(index);
	entry.dwords.lower_dword = *(g_io_apic_base + 4);

	*g_io_apic_base = GET_APIC_REDIRECTION_ENTRY_INDEX(index) + 1;
	entry.dwords.upper_dword = *(g_io_apic_base + 4);

	return entry;
}
static void write_ioapic_redirection_entry(union apic_redirection_entry entry, uint64_t index) {
	*g_io_apic_base = GET_APIC_REDIRECTION_ENTRY_INDEX(index);
	*(g_io_apic_base + 4) = entry.dwords.lower_dword;

	*g_io_apic_base = GET_APIC_REDIRECTION_ENTRY_INDEX(index) + 1;
	*(g_io_apic_base + 4) = entry.dwords.upper_dword;
}

static uint32_t read_lapic_register(enum lapic_registers reg_offset) {
	// division by 4 due to 32 bits integers:
	return g_lapic_base[reg_offset / 4];
}

static void write_lapic_register(enum lapic_registers reg_offset, uint32_t value) {
	// division by 4 due to 32 bits integers:
	g_lapic_base[reg_offset / 4] = value;
}

static void disable_pic() {
	outb(IO_MASTER_PIC_DATA_PORT, 0xff);
	outb(IO_SLAVE_PIC_DATA_PORT, 0xff);
}

static void lapic_timer_handler() {
	if (g_lapic_timer_handler) {
		g_lapic_timer_handler();
	}

	apic_send_eoi();
}

static void print_apic_redirection_entry(union apic_redirection_entry entry) {
	printk("Vector: %d\n", entry.attributes.vector);
	printk("Delivery Mode: %d\n", entry.attributes.delv_mode);
	printk("Destination Mode: %d\n", entry.attributes.dest_mode);
	printk("Delivery Status: %d\n", entry.attributes.delv_status);
	printk("Pin Polarity: %d\n", entry.attributes.pin_polarity);
	printk("Remote IRR: %d\n", entry.attributes.remote_irr);
	printk("Trigger Mode: %d\n", entry.attributes.trigger_mode);
	printk("Mask: %d\n", entry.attributes.mask);
	printk("Destination: %d\n", entry.attributes.destination);
	printk("--------------------------\n");
}
