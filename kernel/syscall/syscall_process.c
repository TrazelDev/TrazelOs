#include <kernel/include/elf_loader.h>
#include <kernel/include/intrrupts.h>
#include <kernel/include/pmm.h>
#include <kernel/include/vfs.h>
#include <kernel/include/vmm.h>

#include "syscall_process.h"

#define USER_STACK_PTR 0x00007FFFFFFFF000

void syscall_execve_handler(struct process_control_block* pcb,
							struct interrupt_info* process_regs) {
	char buffer[4];
	struct vfs_file* file = vfs_open((char*)process_regs->rdi);
	if (file == NULL) {
		process_regs->rax = -1;
		return;
	}

	if (vfs_read(file, (uint8_t*)buffer, 4) != 4 || buffer[0] != 0x7f || buffer[1] != 'E' ||
		buffer[2] != 'L' || buffer[3] != 'F') {
		process_regs->rax = -1;
		vfs_close(file);
		return;
	}
	vfs_close(file);

	void* page_map_physical = vmm_create_new_pagemap();

	uint64_t entry_point = load_elf_to_memory((char*)process_regs->rdi, page_map_physical);

	vmm_map_page(page_map_physical, (void*)(USER_STACK_PTR - REGULAR_PAGE_SIZE), pmm_alloc_page(),
				 MPF_OVERRIDE_CURRENT_PAGING | MPF_WRITABLE_PAGE | MPF_USER_ACCESSIBLE);
	vmm_reload_cr3(page_map_physical);
	vmm_delete_pagemap(pcb->pagemap_hhdm_ptr);

	pcb->pagemap_hhdm_ptr = page_map_physical;

	process_regs->rip = entry_point;
	process_regs->rcx = entry_point;
	process_regs->original_rsp = USER_STACK_PTR;
	process_regs->r11 = 0x202;
}
