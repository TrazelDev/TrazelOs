#include <include/io.h>
#include <kernel/include/cpuid.h>
#include <kernel/include/madt.h>
#include <kernel/include/msr.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>
#include <kernel/include/vmm.h>

#include "kernel/include/apic.h"

#define GET_APIC_REDIRECTION_ENTRY_INDEX(index) (0x10 + ((index) * 2))
uint32_t volatile* g_io_apic_base = NULL;
uint32_t volatile* g_lapic_base = NULL;

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

static union apic_redirection_entry read_apic_redirection_entry(uint64_t index);
static void write_apic_redirection_entry(union apic_redirection_entry entry, uint64_t index);
static void disable_pic();
static void print_apic_redirection_entry(union apic_redirection_entry entry);

void init_apic() {
	if (!cpuid_is_apic_supported()) {
		KERNEL_PANIC("APIC is not supported on this CPU");
	}

	g_io_apic_base = vmm_phys_to_virt_hhdm((void*)madt_get_io_apic_base_addr());
	g_lapic_base = vmm_phys_to_virt_hhdm((void*)madt_get_lapic_base_addr());
	disable_pic();
	g_lapic_base[0xF0 / 4] = 0x100 | 0xFF;	// enable apic

	printk("Initialized apic\n");
}

void apic_send_eoi() { g_lapic_base[0xB0 / 4] = 0; }

void apic_set_legacy_irq_desc_num(enum legacy_isa_irq irq, uint32_t desc_num) {
	uint64_t pin_index = madt_get_gsi_for_irq(irq);

	union apic_redirection_entry entry = read_apic_redirection_entry(pin_index);
	entry.attributes.mask = 0;
	entry.attributes.destination = 0;
	entry.attributes.vector = desc_num;
	write_apic_redirection_entry(entry, pin_index);
}

// module private functions:
// -------------------------------------------------------------------------------------------------

static union apic_redirection_entry read_apic_redirection_entry(uint64_t index) {
	union apic_redirection_entry entry;
	*g_io_apic_base = GET_APIC_REDIRECTION_ENTRY_INDEX(index);
	entry.dwords.lower_dword = *(g_io_apic_base + 4);

	*g_io_apic_base = GET_APIC_REDIRECTION_ENTRY_INDEX(index) + 1;
	entry.dwords.upper_dword = *(g_io_apic_base + 4);

	return entry;
}

static void write_apic_redirection_entry(union apic_redirection_entry entry, uint64_t index) {
	*g_io_apic_base = GET_APIC_REDIRECTION_ENTRY_INDEX(index);
	*(g_io_apic_base + 4) = entry.dwords.lower_dword;

	*g_io_apic_base = GET_APIC_REDIRECTION_ENTRY_INDEX(index) + 1;
	*(g_io_apic_base + 4) = entry.dwords.upper_dword;
}

static void disable_pic() {
	outb(IO_MASTER_PIC_DATA_PORT, 0xff);
	outb(IO_SLAVE_PIC_DATA_PORT, 0xff);
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
