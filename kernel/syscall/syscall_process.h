#pragma once
#include <kernel/include/process_manager.h>

void syscall_execve_handler(struct process_control_block* pcb, struct interrupt_info* process_regs);
