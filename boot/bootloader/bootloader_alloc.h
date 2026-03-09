#pragma once
#include <include/types.h>
// This file implements super basic implementation of malloc

struct basic_allocator {
	void (*init)(void* heap_start, void* heap_end);
	void* (*malloc)(uint64_t);
	void (*free)(void*);
	void* (*free_allocator)(void);
};

struct basic_allocator get_bootloader_basic_allocator(void);
