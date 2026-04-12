#include <include/types.h>
#include <kernel/include/acpi.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "kernel/include/apic.h"
#include "kernel/include/madt.h"

struct madt_table {
	struct acpi_std_header header;
	uint32_t lapic_addr;
	uint32_t flags;
	uint8_t entries[];	// Flexible array members (does not consume struct space)
} __attribute__((packed));

enum madt_entry_type {
	APIC_ENTRY_TYPE_LOCAL_APIC = 0,
	APIC_ENTRY_TYPE_IO_APIC = 1,
	APIC_ENTRY_TYPE_INTERRUPT_SOURCE_OVERRIDE = 2,
};

struct madt_entry_header {
	uint8_t entry_type;
	uint8_t record_len;
} __attribute__((packed));

struct apic_entry_0 {
	struct madt_entry_header header;
	uint8_t acpi_processor_id;
	uint8_t apic_id;
	uint32_t flags;
} __attribute__((packed));

struct apic_entry_1 {
	struct madt_entry_header header;
	uint8_t io_apic_id;
	uint8_t reserved;
	uint32_t io_apic_addr;
	uint16_t flags;
} __attribute__((packed));

struct apic_entry_2 {
	struct madt_entry_header header;
	uint8_t bus_source;
	uint8_t irq_source;
	uint32_t global_system_interrupt;
	uint16_t flags;
} __attribute__((packed));

static bool g_is_initialized = false;
static struct madt_table* g_madt_header = NULL;
static uint64_t g_io_apic_addr = 0;
static uint64_t g_lapic_addr = 0;
static int64_t g_legacy_interrupt_source_override[LIRQ_COUNT];

void init_madt() {
	if (g_is_initialized) {
		return;
	}
	g_is_initialized = true;

	g_madt_header = (struct madt_table*)acpi_get_std_header(ATT_ACPI_TABLE_MADT);
	KERNEL_ASSERT(g_madt_header != NULL, "There is no madt provided by the firmware.");

	g_lapic_addr = g_madt_header->lapic_addr;

	for (int i = 0; i < LIRQ_COUNT; i++) {
		g_legacy_interrupt_source_override[i] = -1;
	}

	uint8_t* table_end = ((uint8_t*)g_madt_header) + g_madt_header->header.length;
	for (uint8_t* current = g_madt_header->entries; current < table_end;
		 current += ((struct madt_entry_header*)current)->record_len) {
		struct madt_entry_header* entry_header = (struct madt_entry_header*)current;

		switch (entry_header->entry_type) {
			case APIC_ENTRY_TYPE_IO_APIC:
				g_io_apic_addr = ((struct apic_entry_1*)current)->io_apic_addr;
				break;
			case APIC_ENTRY_TYPE_INTERRUPT_SOURCE_OVERRIDE: {
				struct apic_entry_2* header_entry2 = (struct apic_entry_2*)current;
				if (header_entry2->bus_source == 0 && header_entry2->irq_source < LIRQ_COUNT) {
					g_legacy_interrupt_source_override[header_entry2->irq_source] =
						header_entry2->global_system_interrupt;
				}
			} break;
			default:
				break;
		}
	}

	printk("Initialized madt successfully. LAPIC addr: 0x%x, IOAPIC addr: 0x%x\n", g_lapic_addr,
		   g_io_apic_addr);
}

uint64_t madt_get_io_apic_base_addr() {
	KERNEL_ASSERT(g_is_initialized, "madt must be initialized before getting ioapic base addr");
	return g_io_apic_addr;
}

uint64_t madt_get_lapic_base_addr() {
	KERNEL_ASSERT(g_is_initialized, "madt must be initialized before getting lapic base addr");
	return g_lapic_addr;
}

uint64_t madt_get_gsi_for_irq(uint64_t irq) {
	KERNEL_ASSERT(irq < LIRQ_COUNT, "Invalid source for interrupt source override lookup");
	KERNEL_ASSERT(g_is_initialized, "madt must be initialized before accessing source overrides");

	if (g_legacy_interrupt_source_override[irq] == -1) {
		return irq;
	}

	return g_legacy_interrupt_source_override[irq];
}

// module private functions:
// -------------------------------------------------------------------------------------------------

void print_entry_0(struct apic_entry_0* entry) {
	printk("entry type: %x\n", entry->header.entry_type);
	printk("record len: %x\n", entry->header.record_len);
	printk("acpi processor id: %x\n", entry->acpi_processor_id);
	printk("apic id: %x\n", entry->apic_id);
	printk("flags: %x\n", entry->flags);
}

void print_entry_1(struct apic_entry_1* entry) {
	printk("entry type: %x\n", entry->header.entry_type);
	printk("record len: %x\n", entry->header.record_len);
	printk("io apic id: %x\n", entry->io_apic_id);
	printk("io apic addr: 0x%x\n", entry->io_apic_addr);
	printk("flags: %x\n", entry->flags);
}

void print_entry_2(struct apic_entry_2* entry) {
	printk("entry type: %x\n", entry->header.entry_type);
	printk("record len: %x\n", entry->header.record_len);
	printk("bus source: %x\n", entry->bus_source);
	printk("irq source: %d\n", entry->irq_source);
	printk("global system interrupt: %x\n", entry->global_system_interrupt);
	printk("flags: %x\n", entry->flags);
}
