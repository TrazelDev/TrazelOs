#include <kernel/include/apic.h>
#include <kernel/include/intrrupts.h>
#include <kernel/include/panic.h>
#include <kernel/include/pmm.h>
#include <kernel/include/printk.h>
#include <kernel/include/queue.h>
#include <kernel/include/vmm.h>

#include "kernel/include/heap.h"
#include "kernel/include/process_manager.h"
#include "scheduler.h"

struct runnable_task {
	struct process_control_block* pcb;
	struct runnable_task* next;
};

static struct runnable_task* g_runnable_tasks_head = NULL;
static struct runnable_task* g_runnable_tasks_rear = NULL;
static struct runnable_task* g_curr_task = NULL;
static bool g_first_task_added = true;

static void timer_scheduler(struct interrupt_info* curr_task_state);
static void task_enqueue(struct runnable_task* task);
static struct runnable_task* task_dequeue();
static inline bool task_queue_empty();

void init_scheduler() { apic_setup_timer_handler(timer_scheduler); }

void scheduler_add_task(struct process_control_block* pcb) {
	struct runnable_task* task = kmalloc(sizeof(struct runnable_task));
	task->pcb = pcb;
	task_enqueue(task);

	if (g_first_task_added) {
		g_curr_task = task_dequeue();
		g_first_task_added = false;
		return;
	}
}

struct process_control_block* pm_get_curr_pcb() { return g_curr_task->pcb; }

// module private functions:
// -------------------------------------------------------------------------------------------------

static void timer_scheduler(struct interrupt_info* curr_task_state) {
	if (task_queue_empty()) {
		return;
	}

	*g_curr_task->pcb->interrupt_info = *curr_task_state;
	task_enqueue(g_curr_task);
	g_curr_task = task_dequeue();
	*curr_task_state = *g_curr_task->pcb->interrupt_info;
	vmm_reload_cr3(g_curr_task->pcb->pagemap_hhdm_ptr);
}

static void task_enqueue(struct runnable_task* task) {
	task->next = NULL;
	if (g_runnable_tasks_rear == NULL) {
		g_runnable_tasks_head = task;
		g_runnable_tasks_rear = task;
		return;
	}

	g_runnable_tasks_rear->next = task;
	g_runnable_tasks_rear = task;
}

static struct runnable_task* task_dequeue() {
	struct runnable_task* next_task = g_runnable_tasks_head;
	if (g_runnable_tasks_head == g_runnable_tasks_rear) {
		g_runnable_tasks_rear = NULL;
	}
	g_runnable_tasks_head = g_runnable_tasks_head->next;

	next_task->next = NULL;
	return next_task;
}

static inline bool task_queue_empty() { return !g_runnable_tasks_head; }
