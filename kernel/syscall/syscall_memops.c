#include <include/types.h>
#include <kernel/include/pmm.h>
#include <kernel/include/vmm.h>

#include "syscall_memops.h"

#define ALIGN_UP(x, align) (((x) % (align) == 0) ? (x) : ((x) + (align) - ((x) % (align))))

void syscall_brk_handler(struct process_control_block* pcb, struct interrupt_info* process_regs) {
	uint64_t prev_heap_addr = (uint64_t)pcb->process_heap_ptr;
	uint64_t new_heap_addr = process_regs->rdi;

	if (new_heap_addr == 0) {
		process_regs->rax = prev_heap_addr;
		return;
	}

	// Not supporting decrementing for now
	// TODO: if the address is smaller than previous one frees memory and returns the smaller addr
	if (new_heap_addr < prev_heap_addr) {
		process_regs->rax = prev_heap_addr;
		return;
	}

	uint64_t start_page = ALIGN_UP(prev_heap_addr, REGULAR_PAGE_SIZE);
	uint64_t end_page = ALIGN_UP(new_heap_addr, REGULAR_PAGE_SIZE);
	for (uint64_t curr_page = start_page; curr_page < end_page; curr_page += REGULAR_PAGE_SIZE) {
		void* phys_page = pmm_alloc_page();
		if (phys_page == NULL) {
			process_regs->rax = prev_heap_addr;
			return;
		}

		int mappage_ret =
			vmm_map_page(pcb->pagemap_hhdm_ptr, (void*)curr_page, phys_page,
						 MPF_WRITABLE_PAGE | MPF_USER_ACCESSIBLE | MPF_OVERRIDE_CURRENT_PAGING);

		if (mappage_ret != 0) {
			process_regs->rax = prev_heap_addr;
			return;
		}
	}

	pcb->process_heap_ptr = (void*)new_heap_addr;
	process_regs->rax = new_heap_addr;
}
