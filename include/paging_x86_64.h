#pragma once
#include <include/types.h>

#define PAGE_TABLE_MAX_LEVEL 4
#define PAGING_LEVEL_MAX_PAGE_COUNT (512ULL * 512 * 512 * 512)
#define VIRTUAL_ADDRESS_OFFSET addr.offset
#define PT_INDEX addr.ptIndex
#define PD_INDEX addr.pdIndex
#define PDPI_INDEX addr.pdpIndex
#define PML4_INDEX addr.plm4Index
#define SIGN_EXTENSION_BITMASK addr.signExtension

union virtual_addr {
	struct {
		uint64_t offset : 12;
		uint64_t pt_index : 9;
		uint64_t pd_index : 9;
		uint64_t pdp_index : 9;
		uint64_t plm4_index : 9;
		uint64_t sign_extension : 16;
	} addr;

	uint64_t raw;
	uint8_t* uint8_ptr;
	uint16_t* uint16_ptr;
	uint32_t* uint32_ptr;
	uint64_t* uint64_ptr;
	void* ptr;
} __attribute__((packed));

#define PAGE_ADDR_TO_PAGE_TABLE_ENTRY_INDEX(x) ((x) >> 12)
#define PAGE_TABLE_ENTRY_INDEX_TO_PAGE_ADDR(x) ((x) << 12)
union page_table_entry {
	struct {
		uint64_t present : 1;		 // page present in memory
		uint64_t writable : 1;		 // read-only if clear, read-write if set
		uint64_t user_access : 1;	 // supervisor level only if clear
		uint64_t write_through : 1;	 // write-through caching enabled if set
		uint64_t cache_disable : 1;	 // cache disabled if set
		uint64_t accessed : 1;		 // page accessed since last refresh
		uint64_t dirty : 1;			 // page written to since last refresh

		// page table with the size (512 * 0x1000 = 2048KB = 2MB ) a typical page is only the size
		// 4KB but this page is special and and it can hold 512 page tables ( 1 page - for table )
		uint64_t huge_page : 1;
		uint64_t global : 1;	// if set, prevents TLB from updating
		uint64_t reserved : 3;	// reserved bits
		uint64_t index : 40;	// index of entry in the pages of the memory
		uint64_t avl2 : 7;
		uint64_t pk : 4;
		uint64_t xd : 1;
	} __attribute__((packed)) attributes;

	uint64_t raw;
} __attribute__((packed));
typedef union page_table_entry page_table_entry_t;
typedef union page_table_entry page_directory_entry_t;
typedef union page_table_entry page_pdp_entry_t;
typedef union page_table_entry page_pml4_entry_t;

inline union virtual_addr create_virtual_addr(void* ptr) {
	union virtual_addr v_addr;
	v_addr.ptr = ptr;
	return v_addr;
}

#define PAGE_TABLE_ENTRIES_IN_ONE_PAGE 512
#define REGULAR_PAGE_SIZE 0x1000
struct page_table {
	union page_table_entry entries[PAGE_TABLE_ENTRIES_IN_ONE_PAGE];
} __attribute__((packed, aligned(REGULAR_PAGE_SIZE)));
typedef struct page_table page_table_t;
typedef struct page_table page_directory_table_t;
typedef struct page_table page_pdp_table_t;
typedef struct page_table page_pml4_table_t;
