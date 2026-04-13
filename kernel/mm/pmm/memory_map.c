#include <include/vendor/limine.h>
#include <kernel/include/printk.h>

#include "memory_map.h"

static void print_entry(struct limine_memmap_entry* entry);

void init_memory_map(volatile struct limine_memmap_request* memmap_request) {
	struct limine_memmap_response* memmap_response = memmap_request->response;

	for (uint32_t i = 0; i < memmap_response->entry_count; i++) {
		print_entry(memmap_response->entries[i]);
	}
}

static void print_entry(struct limine_memmap_entry* entry) {
	printk("Type: %d\n", entry->type);
	printk("Address: %x\n", entry->base);
	printk("Length: %x\n\n", entry->length);
}
