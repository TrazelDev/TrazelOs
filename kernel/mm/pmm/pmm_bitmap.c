#include <include/bitmap.h>
#include <include/mem_utils.h>
#include <include/types.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "kernel/include/pmm.h"

#define BITS_PER_BYTE 8
#define DIV_ROUND_UP(x, align) (((x) + (align) - 1) / (align))
static inline size_t get_ram_max_size(volatile struct limine_memmap_response* memmap_response);
static inline int64_t get_bitmap_region(volatile struct limine_memmap_response* memmap_response,
										size_t physical_ram_size);
static inline void set_page_as_used(void* page_addr);
static inline void set_page_as_unused(void* page_addr);
static void mark_region_as_used(struct limine_memmap_entry* entry);
static void mark_region_as_usable(struct limine_memmap_entry* entry);

static struct tz_bitmap g_bitmap;
static size_t g_hhdm_offset;
void init_pmm(volatile struct limine_memmap_response* memmap_response,
			  volatile struct limine_hhdm_response* hhdm_response) {
	KERNEL_ASSERT(memmap_response != NULL, "Limine memmap response is NULL cannot initialize PMM");
	KERNEL_ASSERT(hhdm_response != NULL, "Limine HHDM response is NULL cannot initialize PMM");
	g_hhdm_offset = hhdm_response->offset;

	// Identifiing a region for the bitmap:
	size_t systems_physical_ram_size = get_ram_max_size(memmap_response);
	int64_t bitmap_region_index = get_bitmap_region(memmap_response, systems_physical_ram_size);
	KERNEL_ASSERT(bitmap_region_index != -1,
				  "Couldn't find a big enough region for the pmm bitmap");
	struct limine_memmap_entry* bitmap_region = memmap_response->entries[bitmap_region_index];

	// Creating the bitmap:
	uint64_t bits_needed_for_bitmap = systems_physical_ram_size / REGULAR_PAGE_SIZE;
	uint64_t bytes_needed_for_bitmap = DIV_ROUND_UP(bits_needed_for_bitmap, BITS_PER_BYTE);
	uint64_t pages_needed_for_bitmap = DIV_ROUND_UP(bytes_needed_for_bitmap, REGULAR_PAGE_SIZE);
	g_bitmap =
		bitmap_create((void*)bitmap_region->base + hhdm_response->offset, bits_needed_for_bitmap);
	memset(g_bitmap.bitmap, 0xFF, bytes_needed_for_bitmap);

	// Marking all the safe usable regions:
	for (uint32_t i = 0; i < memmap_response->entry_count; i++) {
		if (memmap_response->entries[i]->type == LIMINE_MEMMAP_USABLE) {
			mark_region_as_usable(memmap_response->entries[i]);
		}
	}

	for (uint64_t i = 0; i < pages_needed_for_bitmap; i++) {
		set_page_as_used((void*)(bitmap_region->base + (i * REGULAR_PAGE_SIZE)));
	}
	printk("initialized PMM Bitmap! Bitmap located at: 0x%x\n", g_bitmap.bitmap);
}

void* pmm_alloc_page(void) {
	for (uint64_t i = 0; i < g_bitmap.bit_count; i++) {
		if (bitmap_get_bit_value(&g_bitmap, i)) {
			continue;
		}

		void* page_addr = (void*)(i * REGULAR_PAGE_SIZE);
		set_page_as_used(page_addr);
		return page_addr;
	}

	return NULL;
}
void* pmm_alloc_pages(size_t count) {
	KERNEL_ASSERT(count != 0, "PMM can't allocate 0 pages");

	bool found_range = true;
	for (uint64_t i = 0; i <= g_bitmap.bit_count - count; i++) {
		found_range = true;
		for (uint64_t j = i; j < i + count; j++) {
			if (bitmap_get_bit_value(&g_bitmap, j)) {
				found_range = false;
				i = j;
				break;
			}
		}
		if (!found_range) {
			continue;
		}

		for (uint64_t j = i; j < i + count; j++) {
			set_page_as_used((void*)(j * REGULAR_PAGE_SIZE));
		}
		return (void*)(i * REGULAR_PAGE_SIZE);
	}

	return NULL;
}

void* pmm_alloc_page_hhdm(void) {
	void* page = pmm_alloc_page();
	if (page) {
		return page + g_hhdm_offset;
	}
	return page;
}
void* pmm_alloc_pages_hhdm(size_t count) {
	void* pages = pmm_alloc_pages(count);
	if (pages) {
		return pages + g_hhdm_offset;
	}
	return pages;
}

void pmm_free_page(void* physical_addr) { set_page_as_unused(physical_addr); }
void pmm_free_pages(void* physical_addr, size_t count) {
	for (uint64_t i = 0; i < count; i++) {
		set_page_as_unused((void*)((uint64_t)physical_addr + (i * REGULAR_PAGE_SIZE)));
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
	uint64_t bits_needed_for_bitmap = physical_ram_size / REGULAR_PAGE_SIZE;
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
	KERNEL_ASSERT(
		bitmap_set_bit_value(&g_bitmap, (uint64_t)page_addr / REGULAR_PAGE_SIZE, true) != -1,
		"PMM bitmap bit set out of bounds");
}

static inline void set_page_as_unused(void* page_addr) {
	KERNEL_ASSERT(
		bitmap_set_bit_value(&g_bitmap, (uint64_t)page_addr / REGULAR_PAGE_SIZE, false) != -1,
		"PMM bitmap bit set out of bounds");
}

static void mark_region_as_used(struct limine_memmap_entry* entry) {
	for (uint64_t i = 0; i < entry->length / REGULAR_PAGE_SIZE; i++) {
		set_page_as_used((void*)(entry->base + (i * REGULAR_PAGE_SIZE)));
	}
}

static void mark_region_as_usable(struct limine_memmap_entry* entry) {
	for (uint64_t i = 0; i < entry->length / REGULAR_PAGE_SIZE; i++) {
		set_page_as_unused((void*)(entry->base + (i * REGULAR_PAGE_SIZE)));
	}
}
