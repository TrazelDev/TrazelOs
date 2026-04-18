#include <kernel/include/apic.h>
#include <kernel/include/intrrupts.h>
#include <kernel/include/panic.h>
#include <kernel/include/pmm.h>
#include <kernel/include/printk.h>
#include <kernel/include/queue.h>
#include <kernel/include/vmm.h>

#include "kernel/include/heap.h"
#include "kernel/include/scheduler.h"

typedef struct interrupt_info thread_t;

static struct queue g_thread_queue;
thread_t* curr_thread = NULL;
static bool g_is_first_schedule = true;

void timer_scheduler(struct interrupt_info* thread_state) {
	if (g_is_first_schedule) {
		if (queue_is_empty(&g_thread_queue)) {
			return;
		}

		curr_thread = queue_dequeue(&g_thread_queue);
		*thread_state = *curr_thread;
		g_is_first_schedule = false;
		return;
	}

	*curr_thread = *thread_state;
	queue_enqueue(&g_thread_queue, curr_thread);
	curr_thread = queue_dequeue(&g_thread_queue);
	*thread_state = *curr_thread;
}

#define GDT_KERNEL_CS 0x08
#define GDT_KERNEL_DS 0x10

// Bit 9 (0x200) is the Interrupt Enable Flag (IF).
// Bit 1 (0x02) is a CPU reserved bit that must always be 1.
#define RFLAGS_INTERRUPTS_ENABLED 0x202

void init_scheduler() {
	init_queue(&g_thread_queue);
	printk("Initializing processor scheduler\n");
}

void scheduler_add_task(void (*task_func)(void)) {
	thread_t* new_thread = kmalloc(sizeof(thread_t));

	*new_thread = (thread_t){
		.rip = (uint64_t)task_func,
		.code_segment = GDT_KERNEL_CS,
		.rflags = RFLAGS_INTERRUPTS_ENABLED,
		.original_rsp = (uint64_t)(pmm_alloc_page_hhdm() + REGULAR_PAGE_SIZE),
	};

	queue_enqueue(&g_thread_queue, new_thread);
}

void scheduler_handover_execution() { apic_setup_timer_handler(timer_scheduler); }
