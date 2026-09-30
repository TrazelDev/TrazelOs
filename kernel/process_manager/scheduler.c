#include <kernel/include/apic.h>
#include <kernel/include/gdt.h>
#include <kernel/include/intrrupts.h>
#include <kernel/include/panic.h>
#include <kernel/include/pmm.h>
#include <kernel/include/printk.h>
#include <kernel/include/queue.h>
#include <kernel/include/vmm.h>

#include "kernel/include/heap.h"
#include "kernel/include/process_manager.h"
#include "scheduler.h"

static struct task_queue g_runnable_tasks = {NULL, NULL};
static struct process_control_block* g_curr_task = NULL;
static bool g_first_task_added = true;
/* CRITICAL WARNING: This variable is tricky. Do not modify it anywhere other
 * than pm_load_next_ready_process. If something sets it to false from the outside,
 * you risk a race condition where the timer scheduler task switches a kernel process.
 * This skips the swapgs instruction on the way out and will crash the OS on the
 * next syscall.
 */
static volatile bool g_cpu_is_idle = false;

static void timer_scheduler(struct interrupt_info* curr_task_state);
static void task_enqueue(struct task_queue* task_queue, struct process_control_block* task);
static struct process_control_block* task_dequeue(struct task_queue* task_queue);
static inline bool task_queue_empty(struct task_queue* task_queue);

void init_scheduler() { apic_setup_timer_handler(timer_scheduler); }

void scheduler_add_task(struct process_control_block* pcb) {
	task_enqueue(&g_runnable_tasks, pcb);

	if (g_first_task_added) {
		g_curr_task = task_dequeue(&g_runnable_tasks);
		g_first_task_added = false;
		return;
	}
}

struct process_control_block* pm_get_curr_pcb() { return g_curr_task; }

struct pm_wait_queue* pm_create_wait_queue(void) { return kmalloc(sizeof(struct pm_wait_queue)); }
void pm_waitqueue_enqueue(struct pm_wait_queue* wait_queue) {
	g_curr_task->process_state = PS_WAITING_STATE;
	task_enqueue(&wait_queue->wait_queue, g_curr_task);
}
void pm_waitqueue_dequeue_all(struct pm_wait_queue* wait_queue) {
	while (!task_queue_empty(&wait_queue->wait_queue)) {
		struct process_control_block* task = task_dequeue(&wait_queue->wait_queue);
		task->process_state = PS_READY_STATE;
		task_enqueue(&g_runnable_tasks, task);
	}
}
struct interrupt_info* pm_load_next_ready_process() {
	// The current pcb/running task is already at the waiting queue somewhere no need to save it

	while (task_queue_empty(&g_runnable_tasks)) {
		g_cpu_is_idle = true;
		asm volatile("sti; hlt");
		asm volatile("cli");
	}
	g_cpu_is_idle = false;

	g_curr_task = task_dequeue(&g_runnable_tasks);
	vmm_reload_cr3(g_curr_task->pagemap_hhdm_ptr);
	return g_curr_task->interrupt_info;
}

// module private functions:
// -------------------------------------------------------------------------------------------------

static void timer_scheduler(struct interrupt_info* curr_task_state) {
	if (g_cpu_is_idle) {
		return;
	}

	union gdt_segment_selector curr_task_code_segment = {.raw = curr_task_state->code_segment};
	KERNEL_ASSERT(!gdt_is_segment_ring0(curr_task_code_segment),
				  "Scheduler trying to context switch from ring0 process");

	if (task_queue_empty(&g_runnable_tasks) && !g_curr_task->kill_signal) {
		return;
	}

	if (g_curr_task->kill_signal) {
		pm_exit(KILL_SINGLA_EXIT_CODE, g_curr_task, curr_task_state);
	} else {
		*g_curr_task->interrupt_info = *curr_task_state;
		task_enqueue(&g_runnable_tasks, g_curr_task);
	}

	*curr_task_state = *pm_load_next_ready_process();
}

static void task_enqueue(struct task_queue* task_queue, struct process_control_block* task) {
	task->next = NULL;
	if (task_queue->queues_rear == NULL) {
		task_queue->queues_head = task;
		task_queue->queues_rear = task;
		return;
	}

	task_queue->queues_rear->next = task;
	task_queue->queues_rear = task;
}

static struct process_control_block* task_dequeue(struct task_queue* task_queue) {
	struct process_control_block* next_task = task_queue->queues_head;
	if (task_queue->queues_head == task_queue->queues_rear) {
		task_queue->queues_rear = NULL;
	}
	task_queue->queues_head = task_queue->queues_head->next;

	next_task->next = NULL;
	return next_task;
}

static inline bool task_queue_empty(struct task_queue* task_queue) {
	return !task_queue->queues_head;
}
