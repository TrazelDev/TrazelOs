#pragma once
#include <kernel/include/process_manager.h>

#define KILL_SINGLA_EXIT_CODE 139

struct task_queue {
	struct process_control_block* queues_head;
	struct process_control_block* queues_rear;
};

struct pm_wait_queue {
	struct task_queue wait_queue;
};

void init_scheduler();
void scheduler_add_task(struct process_control_block* pcb);
void pm_scheduler_context_switch(struct interrupt_info* curr_task_state);
