#pragma once
#include <include/types.h>

#define MAX_PROCESS_FDS 32
struct process_control_block {
	size_t pid;
	struct interrupt_info* interrupt_info;
	void* pagemap_hhdm_ptr;
	struct vfs_file* fds[MAX_PROCESS_FDS];
};

void init_process_manager();
struct process_control_block* pm_get_curr_pcb();
