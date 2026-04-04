#pragma once
#include <include/vendor/limine.h>

#define REGULAR_PAGE_SIZE 0x1000
void init_pmm(volatile struct limine_memmap_response* memmap_response,
			  volatile struct limine_hhdm_response* hhdm_response);

void* pmm_alloc_page(void);
void* pmm_alloc_pages(size_t count);

/** Allocates a physical page returns the hhdm virt address
 * @return addr to a physical address but in the hhdm (higher half direct mapping) so the kernel
 * could easily read and write for it cause it is already mapped to memory
 */
void* pmm_alloc_page_hhdm(void);
void* pmm_alloc_pages_hhdm(size_t count);

void pmm_free_page(void* physical_addr);
void pmm_free_pages(void* physical_addr, size_t count);
