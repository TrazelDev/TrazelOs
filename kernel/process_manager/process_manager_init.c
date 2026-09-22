#include <kernel/include/elf_loader.h>
#include <kernel/include/pmm.h>
#include <kernel/include/process_manager.h>
#include <kernel/include/vmm.h>

#include "kernel/include/printk.h"

extern void asm_jump_usermode(uint64_t usermode_entrypoint, uint64_t stack_ptr);

#define USER_STACK_PTR 0x00007FFFFFFFF000
void init_process_manager() {
	void* page_map_physical = vmm_create_new_pagemap();
	uint64_t entry_point = load_elf_to_memory("/sbin/init", page_map_physical);

	vmm_map_page(page_map_physical, (void*)(USER_STACK_PTR - REGULAR_PAGE_SIZE), pmm_alloc_page(),
				 MPF_OVERRIDE_CURRENT_PAGING | MPF_WRITABLE_PAGE | MPF_USER_ACCESSIBLE);

	vmm_reload_cr3(page_map_physical);
	asm_jump_usermode(entry_point, USER_STACK_PTR);
}
