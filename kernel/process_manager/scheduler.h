#pragma once
#include <kernel/include/process_manager.h>

void init_scheduler();
void scheduler_add_task(struct process_control_block* pcb);
