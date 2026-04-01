#include <include/paging_x86_64.h>

#include "boot/bootloader/bootloader_print.h"
#include "bootloader_alloc.h"

// We load the kernel at the virtual addr of the following:
// const uint64_t KERNEL_LOADING_POINT = 0xFFFFFFFFC0000000;
// But map a huge page of 0x200000 (2 MiB) to the physical addr 0x0,
// so that the kernel can access the memory below 2 MiB.

static inline void flush_tlb() {
	uint64_t cr3_register;
	asm volatile("movq %%cr3, %0" : "=r"(cr3_register) : : "memory");
	asm volatile("movq %0, %%cr3" : : "r"(cr3_register) : "memory");
}

static inline void flush_tlb_addr(uint64_t addr) {
	asm volatile("invlpg (%0)" ::"r"(addr) : "memory");
}

static inline uint64_t get_cr3_reg() {
	uint64_t cr3_register;
	asm volatile("movq %%cr3, %0" : "=r"(cr3_register) : : "memory");

	return cr3_register;
}

void init_higher_half_kernel(struct basic_allocator* allocator) {
	page_pdp_table_t* pdp_table =
		allocator->malloc_aligned(sizeof(page_pdp_table_t), REGULAR_PAGE_SIZE);
	*pdp_table = (page_pdp_table_t){0};

	page_directory_table_t* page_directory =
		allocator->malloc_aligned(sizeof(page_directory_table_t), REGULAR_PAGE_SIZE);
	*page_directory = (page_directory_table_t){0};

	pdp_table->entries[PAGE_TABLE_ENTRIES_IN_ONE_PAGE - 1] = (page_pdp_entry_t){
		.attributes.present = 1,
		.attributes.writable = 1,
		.attributes.write_through = 1,
		.attributes.huge_page = 0,
		.attributes.index = ((uint64_t)page_directory) >> 12,
	};

	page_directory->entries[0] = (page_directory_entry_t){
		.attributes.present = 1,
		.attributes.writable = 1,
		.attributes.write_through = 1,
		.attributes.huge_page = 1,
		.attributes.index = 0,	// physical addr of 0
	};

	page_pml4_table_t* pml4_table = (page_pml4_table_t*)(get_cr3_reg() & ~0xFFFULL);
	pml4_table->entries[PAGE_TABLE_ENTRIES_IN_ONE_PAGE - 1] = (page_pml4_entry_t){
		.attributes.present = 1,
		.attributes.writable = 1,
		.attributes.write_through = 1,
		.attributes.index = ((uint64_t)pdp_table) >> 12,
	};

	flush_tlb();
}
