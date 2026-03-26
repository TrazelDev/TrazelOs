#include "boot/bootloader/bootloader_print.h"
#include "bootloader_alloc.h"

static uint8_t* s_buffer;
static uint64_t s_buffer_offset;
static uint64_t s_buffer_size;

void init_alloc(void* heap_start, void* heap_end);
void* malloc(uint64_t alloc_size);
void* malloc_aligned(uint64_t size, uint64_t alignment);
void free(void* ptr);
void* free_allocator(void);

struct basic_allocator get_bootloader_basic_allocator(void) {
	struct basic_allocator allocator;
	allocator.init = init_alloc;
	allocator.malloc = malloc;
	allocator.malloc_aligned = malloc_aligned;
	allocator.free = free;
	allocator.free_allocator = free_allocator;
	return allocator;
}

void init_alloc(void* heap_start, void* heap_end) {
	s_buffer = (uint8_t*)heap_start;
	s_buffer_size = (uint64_t)heap_end - (uint64_t)heap_start;
}

void* malloc(uint64_t alloc_size) {
	if (s_buffer_offset + alloc_size > s_buffer_size) {
		print_string("Failed to allocate memory in bootloader allocator\n");
		while (true) {
			asm volatile("hlt");
		}

		return NULL;
	}

	void* allocated_memory = (void*)(s_buffer + s_buffer_offset);
	s_buffer_offset += alloc_size;
	return allocated_memory;
}

void* malloc_aligned(uint64_t size, uint64_t alignment) {
	s_buffer_offset += alignment - (s_buffer_offset % alignment);
	return malloc(size);
}

void free(void* ptr) {
	// the free does nothing in this allocator but exists to satisfy the interface
}

void* free_allocator(void) { return (void*)s_buffer; }
