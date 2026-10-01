#pragma once
#include <kernel/include/intrrupts.h>
#include <kernel/include/process_manager.h>

void syscall_brk_handler(struct process_control_block* pcb, struct interrupt_info* process_regs);
