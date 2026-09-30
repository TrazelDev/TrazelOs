#include <include/types.h>
#include <kernel/include/panic.h>
#include <kernel/include/pmm.h>
#include <kernel/include/printk.h>
#include <kernel/include/vmm.h>

#include "kernel/include/heap.h"

#define ALIGN_UP(x, align) (((x) % (align) == 0) ? (x) : ((x) + (align) - ((x) % (align))))
#define DIV_ROUND_UP(x, align) (((x) + (align) - 1) / (align))
struct memory_block {
	size_t block_size;
	struct memory_block* next;
	struct memory_block* prev;
	uint64_t padding;
};
#define GET_MEMORY_BLOCK_BUFFER(x) ((uint8_t*)((x)) + sizeof(struct memory_block))

/* blocks taken directly from pmm by requesting full pages */
static struct memory_block* g_super_blocks = NULL;
/* blocks that obtained by freeing memory */
static struct memory_block* g_free_blocks = NULL;
static struct memory_block* g_free_block_tail;

/** @return on success a pointer to fitting block. on fail NULL. */
static struct memory_block* find_big_enough_block(struct memory_block* blocks, size_t size);
/** Takes a block and size and allocates memory from the block with a size header at the start.
 * @param size Size of the memory needed for allocatoins without the size needed for the header.
 * @return Pointer to the memory after the header of the memory.
 */
static void* malloc_from_block(struct memory_block* block, size_t size);
static void remove_memory_block_from_list(struct memory_block* block);
static void replace_memory_block_in_list(struct memory_block* old_block,
										 struct memory_block* new_block);

/** Allocating block which are bigger than a REGULAR_PAGE_SIZE therefor too big for standard heap
 * implemtnion */
static void* malloc_largeblock(size_t size);
static void free_largeblock(void* addr);

void init_kernel_heap(void) {
	uint8_t* buffer = pmm_alloc_page_hhdm();
	g_super_blocks = (struct memory_block*)buffer;
	g_super_blocks->block_size = (size_t)REGULAR_PAGE_SIZE - sizeof(struct memory_block);
	g_super_blocks->next = NULL;
	g_super_blocks->prev = NULL;

	printk("Initialized kernel heap\n");
}

void* kmalloc(size_t size) {
	KERNEL_ASSERT(size != 0, "Trying to allocate size 0");
	// KERNEL_PANIC("Function not implemented");
	size = ALIGN_UP(size, sizeof(struct memory_block));

	if (size > REGULAR_PAGE_SIZE - sizeof(struct memory_block)) {
		return malloc_largeblock(size);
	}

	struct memory_block* block = find_big_enough_block(g_free_blocks, size);
	if (block) {
		return malloc_from_block(block, size);
	}

	block = find_big_enough_block(g_super_blocks, size);
	if (block) {
		return malloc_from_block(block, size);
	}

	size_t buffer_page_count = DIV_ROUND_UP(size + sizeof(struct memory_block), REGULAR_PAGE_SIZE);
	uint8_t* buffer = pmm_alloc_pages_hhdm(buffer_page_count);
	if (buffer == NULL) {
		KERNEL_PANIC("Kernel does not have enough memory to allocate that");
	}

	struct memory_block* new_super_block = (struct memory_block*)buffer;
	new_super_block->block_size =
		(buffer_page_count * REGULAR_PAGE_SIZE) - sizeof(struct memory_block);
	new_super_block->next = g_super_blocks;
	new_super_block->prev = NULL;

	if (g_super_blocks) {
		g_super_blocks->prev = new_super_block;
	}
	g_super_blocks = new_super_block;

	return malloc_from_block(g_super_blocks, size);
}
void kfree(void* addr) {
	struct memory_block* addr_header =
		(struct memory_block*)(((uint8_t*)addr) - sizeof(struct memory_block));

	if (addr_header->block_size > REGULAR_PAGE_SIZE - sizeof(struct memory_block)) {
		free_largeblock(addr);
		return;
	}

	addr_header->next = NULL;
	if (!g_free_blocks) {
		addr_header->prev = NULL;
		g_free_blocks = addr_header;
		g_free_block_tail = addr_header;
		return;
	}

	addr_header->prev = g_free_block_tail;
	g_free_block_tail->next = addr_header;
	g_free_block_tail = addr_header;
}

// module private functions:
// -------------------------------------------------------------------------------------------------

static struct memory_block* find_big_enough_block(struct memory_block* blocks, size_t size) {
	struct memory_block* curr_block = blocks;
	while (curr_block) {
		if (curr_block->block_size >= size) {
			return curr_block;
		}
		curr_block = curr_block->next;
	}

	return NULL;
}

static void* malloc_from_block(struct memory_block* block, size_t size) {
	KERNEL_ASSERT(block->block_size >= size,
				  "Trying to allocate from a memory block with not enough size");

	if (block->block_size <= size + (2 * sizeof(struct memory_block))) {
		remove_memory_block_from_list(block);
		return GET_MEMORY_BLOCK_BUFFER(block);
	}

	struct memory_block* original_block_replacement =
		(struct memory_block*)(GET_MEMORY_BLOCK_BUFFER(block) + size);
	original_block_replacement->block_size =
		block->block_size - (size + sizeof(struct memory_block));
	replace_memory_block_in_list(block, original_block_replacement);

	block->block_size = size;
	return GET_MEMORY_BLOCK_BUFFER(block);
}

static void remove_memory_block_from_list(struct memory_block* block) {
	// Remove head:
	if (block->prev == NULL) {
		if (block == g_free_blocks) {
			g_free_blocks = g_free_blocks->next;

			if (g_free_blocks) {
				g_free_blocks->prev = NULL;
			} else {
				g_free_block_tail = NULL;
			}
			return;
		}

		if (block == g_super_blocks) {
			g_super_blocks = g_super_blocks->next;
			if (g_super_blocks) {
				g_super_blocks->prev = NULL;
			}
			return;
		}

		KERNEL_PANIC("Unknown memory block");
	}

	// Removing the node from the list:
	block->prev->next = block->next;
	if (block->next) {
		block->next->prev = block->prev;
	} else if (block == g_free_block_tail) {
		g_free_block_tail = block->prev;
	}
}
static void replace_memory_block_in_list(struct memory_block* old_block,
										 struct memory_block* new_block) {
	new_block->next = old_block->next;
	new_block->prev = old_block->prev;

	// Replace the previous pointer:
	if (new_block->prev != NULL) {
		new_block->prev->next = new_block;
	} else {
		if (old_block == g_free_blocks) {
			g_free_blocks = new_block;
		} else if (old_block == g_super_blocks) {
			g_super_blocks = new_block;
		} else {
			KERNEL_PANIC("Unknown memory block");
		}
	}

	// Fix the pointer after:
	if (new_block->next != NULL) {
		new_block->next->prev = new_block;
	} else {
		if (old_block == g_free_block_tail) {
			g_free_block_tail = new_block;
		}
	}
}

static void* malloc_largeblock(size_t size) {
	size = ALIGN_UP(size + sizeof(struct memory_block), REGULAR_PAGE_SIZE);
	size_t block_pagecount = size / REGULAR_PAGE_SIZE;

	struct memory_block* largeblock = pmm_alloc_pages_hhdm(block_pagecount);
	KERNEL_ASSERT(largeblock != NULL, "Kernel does not have enough memory to allocate that");

	largeblock->next = NULL;
	largeblock->prev = NULL;
	largeblock->padding = 0;
	largeblock->block_size = size - sizeof(struct memory_block);

	return GET_MEMORY_BLOCK_BUFFER(largeblock);
}

static void free_largeblock(void* addr) {
	struct memory_block* addr_header =
		(struct memory_block*)(((uint8_t*)addr) - sizeof(struct memory_block));

	size_t block_totalsize = addr_header->block_size + sizeof(struct memory_block);
	size_t block_pagecount = (block_totalsize) / REGULAR_PAGE_SIZE;
	void* block_phys_addr = vmm_virt_hhdm_to_phys(addr_header);

	pmm_free_pages(block_phys_addr, block_pagecount);
}
