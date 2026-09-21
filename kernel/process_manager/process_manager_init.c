#include <kernel/include/elf_loader.h>
#include <kernel/include/process_manager.h>
#include <kernel/include/vmm.h>

#include "kernel/include/printk.h"

extern void asm_jump_usermode(uint64_t usermode_entrypoint);

void init_process_manager() {
	void* page_map_physical = vmm_create_new_pagemap();
	uint64_t entry_point = load_elf_to_memory("init", page_map_physical);

	vmm_reload_cr3(page_map_physical);
	asm_jump_usermode(entry_point);
}
