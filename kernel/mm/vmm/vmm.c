#include <include/mem_utils.h>
#include <kernel/include/panic.h>
#include <kernel/include/pmm.h>
#include <kernel/include/printk.h>

#include "include/paging_x86_64.h"
#include "kernel/include/vmm.h"

static size_t g_hhdm_offset;
union map_page_flags {
	struct {
		uint64_t override_page : 1; /* Overrides the current value of the page no matter what */
		uint64_t writable_page : 1;
		uint64_t user_accessible : 1;
	} flags;
	uint64_t raw;
};

static inline struct page_table* get_next_page_table(struct page_table* curr_page_table,
													 size_t curr_page_table_index);
static union page_table_entry allocate_page_table_page();
static inline void flush_tlb();
static inline void flush_tlb_addr(uint64_t addr);
static void recursive_pagemap_delete(struct page_table* curr_page_table, uint32_t level);

void init_vmm(volatile struct limine_hhdm_response* hhdm_response) {
	KERNEL_ASSERT(hhdm_response != NULL, "There should be hhdm enabled");
	g_hhdm_offset = hhdm_response->offset;
	printk("Initialized vmm\n");
}

void* vmm_phys_to_virt_hhdm(void* paddr) { return (void*)((uint64_t)paddr + g_hhdm_offset); }
void* vmm_virt_hhdm_to_phys(void* vaddr) { return (void*)((uint64_t)vaddr - g_hhdm_offset); }

void* vmm_get_curr_pagemap() {
	uint64_t cr3_register;
	asm volatile("movq %%cr3, %0" : "=r"(cr3_register) : : "memory");
	uint64_t pml4_phys = cr3_register & ~0xFFF;

	return (void*)(pml4_phys + g_hhdm_offset);
}

void vmm_reload_cr3(void* new_pagemap_hhdm) {
	uint64_t cr3_phys = (uint64_t)new_pagemap_hhdm - g_hhdm_offset;
	asm volatile("movq %0, %%cr3" : : "r"(cr3_phys) : "memory");
}

int vmm_map_page(void* pagemap, void* vaddr, void* paddr, enum vmm_map_page_flags flags) {
	union map_page_flags map_page_flags = {.raw = flags};

	union virtual_addr virtual_addr = (union virtual_addr){.raw = (uint64_t)vaddr};
	uint64_t vaddr_page_table_indexes[] = {
		virtual_addr.addr.plm4_index,
		virtual_addr.addr.pdp_index,
		virtual_addr.addr.pd_index,
		virtual_addr.addr.pt_index,
	};

	page_pml4_table_t* pml4 = pagemap;
	struct page_table* curr_page_table = pml4;
	uint64_t curr_page_table_index = 0;
	for (uint32_t i = 0; i < 3; i++) {
		curr_page_table_index = vaddr_page_table_indexes[i];
		if (curr_page_table->entries[curr_page_table_index].raw == 0) {
			curr_page_table->entries[curr_page_table_index] = allocate_page_table_page();
		}

		if (map_page_flags.flags.user_accessible) {
			curr_page_table->entries[curr_page_table_index].attributes.user_access = 1;
		}

		curr_page_table = get_next_page_table(curr_page_table, curr_page_table_index);
	}

	struct page_table* page_table_final_level = curr_page_table;
	if (!map_page_flags.flags.override_page &&
		curr_page_table->entries[virtual_addr.addr.pt_index].raw != 0) {
		return -1;
	}

	page_table_final_level->entries[virtual_addr.addr.pt_index] = (union page_table_entry){
		.attributes.present = 1,
		.attributes.writable = map_page_flags.flags.writable_page,
		.attributes.user_access = map_page_flags.flags.user_accessible,
		.attributes.write_through = 1,
		.attributes.cache_disable = 0,
		.attributes.accessed = 0,
		.attributes.dirty = 0,
		.attributes.huge_page = 0,
		.attributes.global = 0,
		.attributes.reserved = 0,
		.attributes.index = PAGE_ADDR_TO_PAGE_TABLE_ENTRY_INDEX((uint64_t)paddr),
		.attributes.avl2 = 0,
		.attributes.pk = 0,
		.attributes.xd = 0,
	};

	flush_tlb_addr((uint64_t)vaddr);
	return 0;
}
int vmm_unmap_page(void* pagemap, void* vaddr, uint64_t flags) {
	union virtual_addr virtual_addr = (union virtual_addr){.raw = (uint64_t)vaddr};
	uint64_t vaddr_page_table_indexes[] = {
		virtual_addr.addr.plm4_index,
		virtual_addr.addr.pdp_index,
		virtual_addr.addr.pd_index,
		virtual_addr.addr.pt_index,
	};

	page_pml4_table_t* pml4 = pagemap;
	struct page_table* curr_page_table = pml4;
	uint64_t curr_page_table_index = 0;
	for (uint32_t i = 0; i < 3; i++) {
		curr_page_table_index = vaddr_page_table_indexes[i];
		if (curr_page_table->entries[curr_page_table_index].raw == 0) {
			return -1;	// The entry does not exist so no reason to do anything
		}

		curr_page_table = get_next_page_table(curr_page_table, curr_page_table_index);
	}

	struct page_table* page_table_final_level = curr_page_table;
	if (page_table_final_level->entries[virtual_addr.addr.pt_index].raw == 0) {
		return -1;	// Entry does not exist
	}

	page_table_final_level->entries[virtual_addr.addr.pt_index].raw = 0;
	flush_tlb_addr((uint64_t)vaddr);
	return 0;
}

void* vmm_create_new_pagemap() {
	void* curr_pagemap = vmm_get_curr_pagemap();
	void* new_page_map = pmm_alloc_page_hhdm();
	KERNEL_ASSERT(new_page_map != NULL, "Kernel ran out of memory");

	memcpy(((uint8_t*)new_page_map) + (REGULAR_PAGE_SIZE / 2),
		   ((uint8_t*)curr_pagemap) + (REGULAR_PAGE_SIZE / 2), REGULAR_PAGE_SIZE / 2);
	memset((new_page_map), 0, (REGULAR_PAGE_SIZE / 2));

	return new_page_map;
}

void vmm_delete_pagemap(void* pagemap_hhdm) {
	struct page_table* curr_page_table = (struct page_table*)pagemap_hhdm;
	recursive_pagemap_delete(curr_page_table, 4);
	pmm_free_page((uint8_t*)curr_page_table - g_hhdm_offset);
}

void recursive_pagemap_clone(struct page_table* curr_page_table, struct page_table* new_page_table,
							 uint32_t level) {
	if (level == 0) {
		return;
	}

	uint64_t level_entries = REGULAR_PAGE_SIZE / sizeof(union page_table_entry);
	if (level == 4) {
		level_entries /= 2;
	}

	for (uint64_t i = 0; i < level_entries; i++) {
		union page_table_entry* entry = &(curr_page_table->entries[i]);
		if (entry->raw == 0) {
			continue;
		}
		KERNEL_ASSERT(entry->attributes.present,
					  "Page table entry should be present if it is non-zero");
		KERNEL_ASSERT(!entry->attributes.huge_page, "Huge pages are not supported");

		// Creating an empty page:
		void* new_page_phys = pmm_alloc_page();
		void* new_page_hhdm = vmm_phys_to_virt_hhdm(new_page_phys);
		memset(new_page_hhdm, 0, REGULAR_PAGE_SIZE);

		// Setting up the new page table entry:
		new_page_table->entries[i] = curr_page_table->entries[i];
		new_page_table->entries[i].attributes.index =
			PAGE_ADDR_TO_PAGE_TABLE_ENTRY_INDEX((uint64_t)new_page_phys);

		// recursive call:
		if (level != 1) {
			struct page_table* next_page_table = get_next_page_table(curr_page_table, i);
			struct page_table* new_next_page_table = get_next_page_table(new_page_table, i);
			recursive_pagemap_clone(next_page_table, new_next_page_table, level - 1);
			continue;
		}

		// final page copy:
		void* original_page_phys =
			(void*)(PAGE_TABLE_ENTRY_INDEX_TO_PAGE_ADDR((uint64_t)entry->attributes.index));
		void* original_page_hhdm = vmm_phys_to_virt_hhdm(original_page_phys);
		memcpy(new_page_hhdm, original_page_hhdm, REGULAR_PAGE_SIZE);
	}
}

void* vmm_clone_pagemap(void* pagemap_hhdm) {
	struct page_table* new_page_map = vmm_create_new_pagemap();
	recursive_pagemap_clone((struct page_table*)pagemap_hhdm, new_page_map, 4);

	return new_page_map;
}

// module private functions:
// -------------------------------------------------------------------------------------------------

static inline struct page_table* get_next_page_table(struct page_table* curr_page_table,
													 size_t curr_page_table_index) {
	uint64_t page_index = curr_page_table->entries[curr_page_table_index].attributes.index;
	uint64_t page_paddr = PAGE_TABLE_ENTRY_INDEX_TO_PAGE_ADDR(page_index);
	uint64_t page_kernel_hhdm_vaddr = page_paddr + g_hhdm_offset;

	return (struct page_table*)(page_kernel_hhdm_vaddr);
}

union page_table_entry allocate_page_table_page() {
	void* phys_addr = pmm_alloc_page();
	KERNEL_ASSERT(phys_addr != NULL, "Kernel ran out of memory");
	struct page_table* new_page_table = (struct page_table*)((uint64_t)phys_addr + g_hhdm_offset);
	memset(new_page_table, 0, sizeof(struct page_table));

	return (union page_table_entry){
		.attributes.present = 1,
		.attributes.writable = 1,
		.attributes.user_access = 1,
		.attributes.write_through = 1,
		.attributes.cache_disable = 0,
		.attributes.accessed = 0,
		.attributes.dirty = 0,
		.attributes.huge_page = 0,
		.attributes.global = 0,
		.attributes.reserved = 0,
		.attributes.index = PAGE_ADDR_TO_PAGE_TABLE_ENTRY_INDEX((uint64_t)phys_addr),
		.attributes.avl2 = 0,
		.attributes.pk = 0,
		.attributes.xd = 0,
	};
}

static inline void flush_tlb() {
	uint64_t cr3_register;
	asm volatile("movq %%cr3, %0" : "=r"(cr3_register) : : "memory");
	asm volatile("movq %0, %%cr3" : : "r"(cr3_register) : "memory");
}

static inline void flush_tlb_addr(uint64_t vaddr) {
	asm volatile("invlpg (%0)" ::"r"(vaddr) : "memory");
}

static void recursive_pagemap_delete(struct page_table* curr_page_table, uint32_t level) {
	if (level == 0) {
		return;
	}

	uint64_t level_entries = REGULAR_PAGE_SIZE / sizeof(union page_table_entry);
	if (level == 4) {
		level_entries /= 2;
	}

	for (uint64_t i = 0; i < level_entries; i++) {
		union page_table_entry* entry = &(curr_page_table->entries[i]);
		if (entry->raw == 0) {
			continue;
		}
		if (!entry->attributes.present) {
			entry->raw = 0;
			continue;
		}

		struct page_table* next_page_table = get_next_page_table(curr_page_table, i);
		if (!entry->attributes.huge_page || level == 1) {
			recursive_pagemap_delete(next_page_table, level - 1);
			pmm_free_page((uint8_t*)next_page_table - g_hhdm_offset);
			entry->raw = 0;
			continue;
		}

		uint64_t page_size = (level == 3) ? SUPER_HUGE_PAGE_SIZE : HUGE_PAGE_SIZE;
		for (uint64_t j = 0; j < page_size / REGULAR_PAGE_SIZE; j++) {
			void* page_addr = ((uint8_t*)next_page_table) + (j * REGULAR_PAGE_SIZE);
			pmm_free_page((uint8_t*)page_addr - g_hhdm_offset);
		}
		entry->raw = 0;
	}
}
