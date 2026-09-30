#include <include/mem_utils.h>
#include <kernel/include/elf_loader.h>
#include <kernel/include/heap.h>
#include <kernel/include/intrrupts.h>
#include <kernel/include/panic.h>
#include <kernel/include/pmm.h>
#include <kernel/include/printk.h>
#include <kernel/include/vfs.h>
#include <kernel/include/vmm.h>

#include "kernel/include/process_manager.h"
#include "scheduler.h"

// Bit 9 (0x200) is the Interrupt Enable Flag (IF).
// Bit 1 (0x02) is a CPU reserved bit that must always be 1.
#define RFLAGS_INTERRUPTS_ENABLED 0x202
#define USER_STACK_PTR 0x00007FFFFFFFF000
#define MAX_PIDS 0x1000

struct process_control_block* g_process_list[MAX_PIDS];

extern void asm_jump_usermode(uint64_t usermode_entrypoint, uint64_t stack_ptr);

static size_t generate_pid();

void init_process_manager() {
	struct process_control_block* init_process_pcb = kmalloc(sizeof(struct process_control_block));
	init_process_pcb->pid = generate_pid();
	init_process_pcb->ppid = 0;
	init_process_pcb->process_state = PS_READY_STATE;

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

	init_process_pcb->interrupt_info = kmalloc(sizeof(struct interrupt_info));

	g_process_list[init_process_pcb->pid] = init_process_pcb;
	scheduler_add_task(init_process_pcb);
	printk("Initializing processor scheduler and jumping to user mode init process\n\n\n");

	init_scheduler();
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
	process_regs->r11 = RFLAGS_INTERRUPTS_ENABLED;

	vfs_close(file);
	return 0;
}

int pm_fork(struct process_control_block* pcb, struct interrupt_info* process_regs) {
	struct process_control_block* child_pcb = kmalloc(sizeof(struct process_control_block));
	child_pcb->pid = generate_pid();
	child_pcb->ppid = pcb->pid;
	child_pcb->process_state = PS_READY_STATE;

	for (uint64_t i = 0; i < MAX_PROCESS_FDS; i++) {
		child_pcb->fds[i] = pcb->fds[i];
		if (child_pcb->fds[i] != NULL) {
			child_pcb->fds[i]->file_ref_count++;
		}
	}

	child_pcb->interrupt_info = kmalloc(sizeof(struct interrupt_info));
	*child_pcb->interrupt_info = *process_regs;

	child_pcb->pagemap_hhdm_ptr = vmm_clone_pagemap(pcb->pagemap_hhdm_ptr);

	child_pcb->interrupt_info->rax = 0;	 // telling the child process it is not the parent

	g_process_list[child_pcb->pid] = child_pcb;
	scheduler_add_task(child_pcb);
	return (int)child_pcb->pid;
}

struct process_control_block* pm_get_pcb_by_pid(size_t pid) {
	if (pid >= MAX_PIDS) {
		return NULL;
	}

	return g_process_list[pid];
}

// module private functions:
// -------------------------------------------------------------------------------------------------

static size_t generate_pid() {
	for (size_t i = 1; i < MAX_PIDS; i++) {
		if (g_process_list[i] == NULL) {
			return i;
		}
	}

	KERNEL_PANIC("Too many processes running at once. MAX_PIDS limit reached");
}
