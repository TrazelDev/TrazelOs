#include "bootloader_alloc.h"

static uint8_t* s_buffer;
static uint64_t s_buffer_offset;
static uint64_t s_buffer_size;

void init_alloc(void* heap_start, void* heap_end);
void* malloc(uint64_t alloc_size);
void* realloc(uint64_t alloc_size);
void* free_allocator(void);

struct basic_allocator get_bootloader_basic_allocator(void) {
	struct basic_allocator allocator;
	allocator.init = init_alloc;
	allocator.malloc = malloc;
	allocator.free_allocator = free_allocator;
	return allocator;
}

void init_alloc(void* heap_start, void* heap_end) {
	s_buffer = (uint8_t*)heap_start;
	s_buffer_size = (uint64_t)heap_end - (uint64_t)heap_start;
}

void* malloc(uint64_t alloc_size) {
	if (s_buffer_offset + alloc_size > s_buffer_size) {
		return NULL;
	}

	void* allocated_memory = (void*)(s_buffer + s_buffer_offset);
	s_buffer_offset += alloc_size;
	return allocated_memory;
}

void* free_allocator(void) { return (void*)s_buffer; }
