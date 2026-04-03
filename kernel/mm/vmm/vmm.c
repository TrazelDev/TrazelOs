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

void init_vmm(volatile struct limine_hhdm_response* hhdm_response) {
	KERNEL_ASSERT(hhdm_response != NULL, "There should be hhdm enabled");
	g_hhdm_offset = hhdm_response->offset;
	printk("Initialized vmm\n");
}

void* vmm_get_curr_pagemap() {
	uint64_t cr3_register;
	asm volatile("movq %%cr3, %0" : "=r"(cr3_register) : : "memory");
	uint64_t pml4_phys = cr3_register & ~0xFFF;

	return (void*)(pml4_phys + g_hhdm_offset);
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
