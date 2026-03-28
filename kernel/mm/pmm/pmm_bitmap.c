#include <include/bitmap.h>
#include <include/mem_utils.h>
#include <include/types.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "kernel/include/pmm.h"

#define PAGE_SIZE 0x1000
#define BITS_PER_BYTE 8
#define DIV_ROUND_UP(x, align) (((x) + (align) - 1) / (align))
static inline size_t get_ram_max_size(volatile struct limine_memmap_response* memmap_response);
static inline int64_t get_bitmap_region(volatile struct limine_memmap_response* memmap_response,
										size_t physical_ram_size);
static inline void set_page_as_used(void* page_addr);
static inline void set_page_as_unused(void* page_addr);
static void mark_region_as_used(struct limine_memmap_entry* entry);
static void mark_region_as_usable(struct limine_memmap_entry* entry);

static struct tz_bitmap s_bitmap;
void init_pmm(volatile struct limine_memmap_response* memmap_response,
			  volatile struct limine_hhdm_response* hhdm_response) {
	KERNEL_ASSERT(memmap_response != NULL, "Limine memmap response is NULL cannot initialize PMM");
	KERNEL_ASSERT(hhdm_response != NULL, "Limine HHDM response is NULL cannot initialize PMM");

	// Identifiing a region for the bitmap:
	size_t systems_physical_ram_size = get_ram_max_size(memmap_response);
	int64_t bitmap_region_index = get_bitmap_region(memmap_response, systems_physical_ram_size);
	KERNEL_ASSERT(bitmap_region_index != -1,
				  "Couldn't find a big enough region for the pmm bitmap");
	struct limine_memmap_entry* bitmap_region = memmap_response->entries[bitmap_region_index];

	// Creating the bitmap:
	uint64_t bits_needed_for_bitmap = systems_physical_ram_size / PAGE_SIZE;
	uint64_t bytes_needed_for_bitmap = DIV_ROUND_UP(bits_needed_for_bitmap, BITS_PER_BYTE);
	uint64_t pages_needed_for_bitmap = DIV_ROUND_UP(bytes_needed_for_bitmap, PAGE_SIZE);
	s_bitmap =
		bitmap_create((void*)bitmap_region->base + hhdm_response->offset, bits_needed_for_bitmap);
	memset(s_bitmap.bitmap, 0xFF, bytes_needed_for_bitmap);

	// Marking all the safe usable regions:
	for (uint32_t i = 0; i < memmap_response->entry_count; i++) {
		if (memmap_response->entries[i]->type == LIMINE_MEMMAP_USABLE) {
			mark_region_as_usable(memmap_response->entries[i]);
		}
	}

	for (uint64_t i = 0; i < pages_needed_for_bitmap; i++) {
		set_page_as_used((void*)(bitmap_region->base + (i * PAGE_SIZE)));
	}
	printk("initialized PMM Bitmap! Bitmap located at: 0x%x\n", s_bitmap.bitmap);
}

void* pmm_alloc_page(void) {
	for (uint64_t i = 0; i < s_bitmap.bit_count; i++) {
		if (bitmap_get_bit_value(&s_bitmap, i)) {
			continue;
		}

		void* page_addr = (void*)(i * PAGE_SIZE);
		set_page_as_used(page_addr);
		return page_addr;
	}

	return NULL;
}
void* pmm_alloc_pages(size_t count) {
	KERNEL_ASSERT(count != 0, "PMM can't allocate 0 pages");

	bool found_range = true;
	for (uint64_t i = 0; i <= s_bitmap.bit_count - count; i++) {
		found_range = true;
		for (uint64_t j = i; j < i + count; j++) {
			if (bitmap_get_bit_value(&s_bitmap, j)) {
				found_range = false;
				i = j;
				break;
			}
		}
		if (!found_range) {
			continue;
		}

		for (uint64_t j = i; j < i + count; j++) {
			set_page_as_used((void*)(j * PAGE_SIZE));
		}
		return (void*)(i * PAGE_SIZE);
	}

	return NULL;
}

void pmm_free_page(void* physical_addr) { set_page_as_unused(physical_addr); }
void pmm_free_pages(void* physical_addr, size_t count) {
	for (uint64_t i = 0; i < count; i++) {
		set_page_as_unused((void*)((uint64_t)physical_addr + (i * PAGE_SIZE)));
	}
}

static inline size_t get_ram_max_size(volatile struct limine_memmap_response* memmap_response) {
	size_t max_address = 0;
	for (uint32_t i = 0; i < memmap_response->entry_count; i++) {
		size_t top = memmap_response->entries[i]->base + memmap_response->entries[i]->length;
		if (top > max_address) {
			max_address = top;
		}
	}

	return max_address;
}

static inline int64_t get_bitmap_region(volatile struct limine_memmap_response* memmap_response,
										size_t physical_ram_size) {
	uint64_t bits_needed_for_bitmap = physical_ram_size / PAGE_SIZE;
	uint64_t bytes_needed_for_bitmap = DIV_ROUND_UP(bits_needed_for_bitmap, BITS_PER_BYTE);

	for (uint32_t i = 0; i < memmap_response->entry_count; i++) {
		if (memmap_response->entries[i]->type != LIMINE_MEMMAP_USABLE) {
			continue;
		}

		if (memmap_response->entries[i]->length >= bytes_needed_for_bitmap) {
			return i;
		}
	}

	return -1;
}

static inline void set_page_as_used(void* page_addr) {
	KERNEL_ASSERT(bitmap_set_bit_value(&s_bitmap, (uint64_t)page_addr / PAGE_SIZE, true) != -1,
				  "PMM bitmap bit set out of bounds");
}

static inline void set_page_as_unused(void* page_addr) {
	KERNEL_ASSERT(bitmap_set_bit_value(&s_bitmap, (uint64_t)page_addr / PAGE_SIZE, false) != -1,
				  "PMM bitmap bit set out of bounds");
}

static void mark_region_as_used(struct limine_memmap_entry* entry) {
	for (uint64_t i = 0; i < entry->length / PAGE_SIZE; i++) {
		set_page_as_used((void*)(entry->base + (i * PAGE_SIZE)));
	}
}

static void mark_region_as_usable(struct limine_memmap_entry* entry) {
	for (uint64_t i = 0; i < entry->length / PAGE_SIZE; i++) {
		set_page_as_unused((void*)(entry->base + (i * PAGE_SIZE)));
	}
}
