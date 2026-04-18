#include <kernel/include/apic.h>
#include <kernel/include/intrrupts.h>
#include <kernel/include/panic.h>
#include <kernel/include/pmm.h>
#include <kernel/include/printk.h>
#include <kernel/include/queue.h>
#include <kernel/include/vmm.h>

#include "kernel/include/heap.h"
#include "kernel/include/scheduler.h"

#define GDT_KERNEL_CS 0x08
#define GDT_KERNEL_DS 0x10
// Bit 9 (0x200) is the Interrupt Enable Flag (IF).
// Bit 1 (0x02) is a CPU reserved bit that must always be 1.
#define RFLAGS_INTERRUPTS_ENABLED 0x202
typedef struct interrupt_info thread_state_t;

struct runnable_task {
	thread_state_t thread_state;
	struct runnable_task* next;
};

static struct runnable_task* g_runnable_tasks_head = NULL;
static struct runnable_task* g_runnable_tasks_rear = NULL;
static struct runnable_task* g_curr_task = NULL;
static bool g_first_task_scheduled = true;

static void timer_scheduler(struct interrupt_info* thread_state);
static void task_enqueue(struct runnable_task* task);
static struct runnable_task* task_dequeue();
static inline bool task_queue_empty();

void init_scheduler() { printk("Initializing processor scheduler\n"); }

void scheduler_add_task(void (*task_func)(void)) {
	struct runnable_task* task = kmalloc(sizeof(struct runnable_task));

	task->thread_state = (thread_state_t){
		.rip = (uint64_t)task_func,
		.code_segment = GDT_KERNEL_CS,
		.rflags = RFLAGS_INTERRUPTS_ENABLED,
		.original_rsp = (uint64_t)(pmm_alloc_page_hhdm() + REGULAR_PAGE_SIZE),
	};

	task_enqueue(task);
}

void scheduler_handover_execution() { apic_setup_timer_handler(timer_scheduler); }

// module private functions:
// -------------------------------------------------------------------------------------------------

static void timer_scheduler(struct interrupt_info* thread_state) {
	if (task_queue_empty()) {
		return;
	}

	if (g_first_task_scheduled) {
		g_curr_task = task_dequeue();
		*thread_state = g_curr_task->thread_state;
		g_first_task_scheduled = false;
		return;
	}

	g_curr_task->thread_state = *thread_state;
	task_enqueue(g_curr_task);
	g_curr_task = task_dequeue();
	*thread_state = g_curr_task->thread_state;
}

static void task_enqueue(struct runnable_task* task) {
	task->next = NULL;
	if (g_runnable_tasks_rear == NULL) {
		g_runnable_tasks_head = task;
		g_runnable_tasks_rear = task;
	} else {
		g_runnable_tasks_rear->next = task;
		g_runnable_tasks_rear = task;
	}
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
