#pragma once
#include <include/types.h>
#include <include/vendor/limine.h>

enum vmm_map_page_flags {
	NO_FLAGS = 0,
	MPF_OVERRIDE_CURRENT_PAGING = (1 << 0),
	MPF_WRITABLE_PAGE = (1 << 1),
	MPF_USER_ACCESSIBLE = (1 << 2),
};

void init_vmm(volatile struct limine_hhdm_response* hhdm_response);
void* vmm_get_curr_pagemap();
void vmm_reload_cr3(void* new_pagemap_hhdm);

/** Gets a physical addr and returns the corresponding virtual addr in the HHDM for kernel usage
 * Does not verify addr is not NULL
 * @return kernel based hhdm writable addr
 */
void* vmm_phys_to_virt_hhdm(void* paddr);

/** Gets a virtual hhdm kernel addr and returns the corresponding physical addr
 * Does not verify addr is not NULL
 * @return physical addr
 */
void* vmm_virt_hhdm_to_phys(void* vaddr);

int vmm_map_page(void* pagemap, void* vaddr, void* paddr, enum vmm_map_page_flags flags);
int vmm_unmap_page(void* pagemap, void* vaddr, uint64_t flags);

/** Creates a new pagemap
 * @return new pagemap in hhdm form
 */
void* vmm_create_new_pagemap();
void vmm_delete_pagemap(void* pagemap_hhdm);
/** Copies all the kernel top half of the pages, recursively clones all of the pages in the tree
 * hierarchy of the source pagemap_hhdm var into a new pagemap and than copies the actual pages
 * themself as well
 * @return new pagemap in hhdm form
 */
void* vmm_clone_pagemap(void* pagemap_hhdm);

/** @brief Calculates the number of memory pages spanned by some range of 2 addresses
 * Func parameter must satisfy the condition (start_ptr <= end_ptr)
 * @return The number of pages that touch this range (if both address in the same page returns 1) */
size_t vmm_count_spanned_pages(uint64_t start_ptr, uint64_t end_ptr);
