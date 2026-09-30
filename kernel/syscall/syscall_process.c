#include <kernel/include/elf_loader.h>
#include <kernel/include/intrrupts.h>
#include <kernel/include/pmm.h>
#include <kernel/include/process_manager.h>
#include <kernel/include/vfs.h>
#include <kernel/include/vmm.h>

#include "kernel/include/panic.h"
#include "syscall_process.h"

#define USER_STACK_PTR 0x00007FFFFFFFF000

void syscall_execve_handler(struct process_control_block* pcb,
							struct interrupt_info* process_regs) {
	process_regs->rax = pm_execve(pcb, (char*)process_regs->rdi, process_regs);
}

void syscall_fork_handler(struct process_control_block* pcb, struct interrupt_info* process_regs) {
	process_regs->rax = pm_fork(pcb, process_regs);
}

void syscall_getpid_handler(struct process_control_block* pcb,
							struct interrupt_info* process_regs) {
	process_regs->rax = pcb->pid;
}
