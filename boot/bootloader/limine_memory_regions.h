#pragma once
#include <include/types.h>
#include <include/vendor/limine.h>

#include "bootloader_alloc.h"

/** @return size of the physical ram. */
struct limine_memmap_response* init_memory_regions(size_t boot_reserve_mem_size,
												   void* boot_reserve_mem_start,
												   struct basic_allocator* alloc);
