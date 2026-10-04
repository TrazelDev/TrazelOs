#pragma once
#include <kernel/include/intrrupts.h>
#include <kernel/include/process_manager.h>

void syscall_write_handler(struct process_control_block* pcb, struct interrupt_info* process_regs);
void syscall_read_handler(struct process_control_block* pcb, struct interrupt_info* process_regs);
void syscall_open_handler(struct process_control_block* pcb, struct interrupt_info* process_regs);
void syscall_close_handler(struct process_control_block* pcb, struct interrupt_info* process_regs);
void syscall_dup_handler(struct process_control_block* pcb, struct interrupt_info* process_regs);
void syscall_dup2_handler(struct process_control_block* pcb, struct interrupt_info* process_regs);
void syscall_lseek_handler(struct process_control_block* pcb, struct interrupt_info* process_regs);
