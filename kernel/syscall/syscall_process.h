#pragma once
#include <kernel/include/process_manager.h>

void syscall_execve_handler(struct process_control_block* pcb, struct interrupt_info* process_regs);
void syscall_fork_handler(struct process_control_block* pcb, struct interrupt_info* process_regs);
void syscall_getpid_handler(struct process_control_block* pcb, struct interrupt_info* process_regs);
void syscall_getppid_handler(struct process_control_block* pcb,
							 struct interrupt_info* process_regs);
void syscall_exit_handler(struct process_control_block* pcb, struct interrupt_info* process_regs);
void syscall_wait_handler(struct process_control_block* pcb, struct interrupt_info* process_regs);
