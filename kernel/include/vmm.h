#pragma once
#include <include/types.h>
#include <include/vendor/limine.h>

// TODO: Create good unit test for this vmm unit

enum vmm_map_page_flags {
	NO_FLAGS = 0,
	MPF_OVERRIDE_CURRENT_PAGING = (1 << 0),
	MPF_WRITABLE_PAGE = (1 << 1),
	MPF_USER_ACCESSIBLE = (1 << 2),
};

void init_vmm(volatile struct limine_hhdm_response* hhdm_response);
void* vmm_get_curr_pagemap();

int vmm_map_page(void* pagemap, void* vaddr, void* paddr, enum vmm_map_page_flags flags);
int vmm_unmap_page(void* pagemap, void* vaddr, uint64_t flags);
