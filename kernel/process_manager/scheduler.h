#pragma once
#include <kernel/include/process_manager.h>

struct task_queue {
	struct process_control_block* queues_head;
	struct process_control_block* queues_rear;
};

struct pm_wait_queue {
	struct task_queue wait_queue;
};

void init_scheduler();
void scheduler_add_task(struct process_control_block* pcb);
