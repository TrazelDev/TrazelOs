#pragma once
#include <include/types.h>

struct idt_entry;

/* @brief initializing the idt */
void init_cpu_exceptions();

/* @brief load a new interrupt descriptor to the idt
 * @param index the index of the interrupt in the idt
 * @param desc the interrupt descriptor to be loaded
 * @return index on success, other on failure
 */
size_t load_new_interrupt(uint8_t index, struct idt_entry* desc);
