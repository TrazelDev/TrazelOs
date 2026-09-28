#include <kernel/include/elf_loader.h>
#include <kernel/include/heap.h>
#include <kernel/include/intrrupts.h>
#include <kernel/include/pmm.h>
#include <kernel/include/printk.h>
#include <kernel/include/vfs.h>
#include <kernel/include/vmm.h>

#include "kernel/include/panic.h"
#include "kernel/include/process_manager.h"

#define USER_STACK_PTR 0x00007FFFFFFFF000

extern void asm_jump_usermode(uint64_t usermode_entrypoint, uint64_t stack_ptr);
static struct process_control_block* g_curr_pcb;

struct process_control_block* pm_get_curr_pcb() { return g_curr_pcb; }

void init_process_manager() {
	struct process_control_block* init_process_pcb = kmalloc(sizeof(struct process_control_block));
	init_process_pcb->pid = 0;

	// Setup process file descriptors:
	struct vfs_file* tty_dev = vfs_open("/dev/tty");
	KERNEL_ASSERT(tty_dev, "Failed to open /dev/tty for init process file descriptors");
	init_process_pcb->fds[0] = tty_dev;
	init_process_pcb->fds[1] = tty_dev;
	init_process_pcb->fds[2] = tty_dev;
	for (uint64_t i = 3; i < MAX_PROCESS_FDS; i++) {
		init_process_pcb->fds[i] = NULL;
	}

	struct vfs_file* init_file = vfs_open("/sbin/init");
	KERNEL_ASSERT(init_file != NULL,
				  "Failed to open init process file /sbin/init please verify there is an init "
				  "process to boot");

	void* pagemap_hhdm_ptr = vmm_create_new_pagemap();
	uint64_t entry_point = load_elf_to_memory(init_file, pagemap_hhdm_ptr);
	KERNEL_ASSERT(entry_point, "Failed to load init process into memory");

	int mappage_result = vmm_map_page(
		pagemap_hhdm_ptr, (void*)(USER_STACK_PTR - REGULAR_PAGE_SIZE), pmm_alloc_page(),
		MPF_OVERRIDE_CURRENT_PAGING | MPF_WRITABLE_PAGE | MPF_USER_ACCESSIBLE);
	KERNEL_ASSERT(mappage_result == 0, "Failed to map init process stack into memory");

	vmm_reload_cr3(pagemap_hhdm_ptr);
	init_process_pcb->pagemap_hhdm_ptr = pagemap_hhdm_ptr;

	g_curr_pcb = init_process_pcb;
	printk("Initializing processor scheduler and jumping to user mode init process\n\n\n");
	asm_jump_usermode(entry_point, USER_STACK_PTR);
}

int64_t pm_execve(struct process_control_block* pcb, const char* path,
				  struct interrupt_info* process_regs) {
	struct vfs_file* file = vfs_open(path);
	if (file == NULL) {
		return -1;
	}

	void* pagemap_hhdm = vmm_create_new_pagemap();

	uint64_t entry_point = load_elf_to_memory(file, pagemap_hhdm);
	if (entry_point == NULL) {
		vmm_delete_pagemap(pagemap_hhdm);
		vfs_close(file);
		return -1;
	}

	int mappage_result =
		vmm_map_page(pagemap_hhdm, (void*)(USER_STACK_PTR - REGULAR_PAGE_SIZE), pmm_alloc_page(),
					 MPF_OVERRIDE_CURRENT_PAGING | MPF_WRITABLE_PAGE | MPF_USER_ACCESSIBLE);
	if (mappage_result != 0) {
		vmm_delete_pagemap(pagemap_hhdm);
		vfs_close(file);
		return -1;
	}

	vmm_reload_cr3(pagemap_hhdm);
	vmm_delete_pagemap(pcb->pagemap_hhdm_ptr);

	pcb->pagemap_hhdm_ptr = pagemap_hhdm;
	process_regs->rip = entry_point;
	process_regs->rcx = entry_point;
	process_regs->original_rsp = USER_STACK_PTR;
	process_regs->r11 = 0x202;

	vfs_close(file);
	return 0;
}
