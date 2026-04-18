#pragma once

void init_scheduler();
void scheduler_add_task(void (*task_func)(void));
void scheduler_handover_execution();
