#include <kernel/include/elf_loader.h>
#include <kernel/include/heap.h>
#include <kernel/include/pmm.h>
#include <kernel/include/printk.h>
#include <kernel/include/vfs.h>
#include <kernel/include/vmm.h>

#include "kernel/include/process_manager.h"

extern void asm_jump_usermode(uint64_t usermode_entrypoint, uint64_t stack_ptr);
static struct process_control_block* g_curr_pcb;

void init_process_manager() {
#define USER_STACK_PTR 0x00007FFFFFFFF000
	struct process_control_block* init_process_pcb = kmalloc(sizeof(struct process_control_block));
	init_process_pcb->pid = 0;

	// Setup process file descriptors:
	struct vfs_file* tty_dev = vfs_open("/dev/tty");
	init_process_pcb->fds[0] = tty_dev;
	init_process_pcb->fds[1] = tty_dev;
	init_process_pcb->fds[2] = tty_dev;
	for (uint64_t i = 3; i < MAX_PROCESS_FDS; i++) {
		init_process_pcb->fds[i] = NULL;
	}

	// Load process to memory:
	void* page_map_physical = vmm_create_new_pagemap();
	uint64_t entry_point = load_elf_to_memory("/sbin/init", page_map_physical);
	vmm_map_page(page_map_physical, (void*)(USER_STACK_PTR - REGULAR_PAGE_SIZE), pmm_alloc_page(),
				 MPF_OVERRIDE_CURRENT_PAGING | MPF_WRITABLE_PAGE | MPF_USER_ACCESSIBLE);
	vmm_reload_cr3(page_map_physical);
	init_process_pcb->pagemap_hhdm_ptr = page_map_physical;

	g_curr_pcb = init_process_pcb;
	printk("Initializing processor scheduler and jumping to user mode init process\n\n\n");
	asm_jump_usermode(entry_point, USER_STACK_PTR);
#undef USER_STACK_PTR
}

struct process_control_block* pm_get_curr_pcb() { return g_curr_pcb; }
