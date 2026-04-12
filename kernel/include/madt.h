#pragma once
#include <include/types.h>

void init_madt();

/* Returns physical addr of the io apic base */
uint64_t madt_get_io_apic_base_addr();

/* Returns physical addr of the lapic base */
uint64_t madt_get_lapic_base_addr();

/** @brief Takes irq source and returns gsi.
 * @param source The IRQ source number.
 * @return The gsi if found in madt, the irq if not found (which will be the gsi).
 */
uint64_t madt_get_gsi_for_irq(uint64_t irq);
