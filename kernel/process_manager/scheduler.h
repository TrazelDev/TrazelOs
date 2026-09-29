#pragma once
#include <kernel/include/process_manager.h>

struct task_node {
	struct process_control_block* pcb;
	struct task_node* next;
};

struct task_queue {
	struct task_node* queues_head;
	struct task_node* queues_rear;
};

struct pm_wait_queue {
	struct task_queue wait_queue;
};

void init_scheduler();
void scheduler_add_task(struct process_control_block* pcb);
