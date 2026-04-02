#include "limine_memory_regions.h"
#include "vendor/limine.h"

extern uint64_t address_with_memory_maps;
extern uint64_t memory_region_count;

enum memory_region {
	MR_USABLE_RAM = 1,
	MR_RESERVED = 2,
	MR_RECLAIMABLE = 3,
	MR_NON_RECLAIMABLE = 4,
	MR_UNUSABLE = 5,
};

struct memory_map_entry {
	uint64_t base_address;
	uint64_t region_length;
	uint32_t region_type;
	uint32_t extended_attributes;
};

// TODO: Need to remove the 0x100,000-0x200,000 area from free memory.
struct limine_memmap_response* init_memory_regions(size_t boot_reserve_mem_size,
												   void* boot_reserve_mem_start,
												   struct basic_allocator* alloc) {
	// Constant setup:
	const static uint32_t BIOS_MEM_REGIONS_TO_LIMINE_MAP[] = {
		LIMINE_MEMMAP_USABLE,	LIMINE_MEMMAP_RESERVED, LIMINE_MEMMAP_RESERVED,
		LIMINE_MEMMAP_RESERVED, LIMINE_MEMMAP_RESERVED,
	};
	struct memory_map_entry* mem_regions = (struct memory_map_entry*)address_with_memory_maps;

	struct limine_memmap_response* memmap_response =
		alloc->malloc(sizeof(struct limine_memmap_response));
	memmap_response->revision = 0;
	memmap_response->entry_count = memory_region_count + 1;
	memmap_response->entries =
		alloc->malloc((memmap_response->entry_count + 1) * sizeof(struct limine_memmap_entry*));

	// Setting up memory regions from bios:
	for (uint64_t i = 0; i < memory_region_count; i++) {
		struct limine_memmap_entry* response = alloc->malloc(sizeof(struct limine_memmap_entry));
		struct memory_map_entry bios_memmap_entry = mem_regions[i];
		*response = (struct limine_memmap_entry){
			.type = BIOS_MEM_REGIONS_TO_LIMINE_MAP[bios_memmap_entry.region_type],
			.base = bios_memmap_entry.base_address,
			.length = bios_memmap_entry.region_length};
		memmap_response->entries[i] = response;
	}

	// Setting up bootloader reserved memory:
	memmap_response->entries[memory_region_count] =
		alloc->malloc(sizeof(struct limine_memmap_entry));
	*memmap_response->entries[memory_region_count] = (struct limine_memmap_entry){
		.base = (size_t)(boot_reserve_mem_start),
		.length = boot_reserve_mem_size,
		.type = LIMINE_MEMMAP_RESERVED,
	};

	return memmap_response;
}
