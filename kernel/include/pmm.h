#pragma once
#include <include/vendor/limine.h>

void init_pmm(volatile struct limine_memmap_response* memmap_response,
			  volatile struct limine_hhdm_response* hhdm_response);

void* pmm_alloc_page(void);
void* pmm_alloc_pages(size_t count);

void pmm_free_page(void* physical_addr);
void pmm_free_pages(void* physical_addr, size_t count);
