#pragma once
#include <include/types.h>

struct pm_wait_queue;
enum process_state { PS_READY_STATE, PS_WAITING_STATE, PS_ZOMBIE_STATE };

#define MAX_PROCESS_FDS 32
struct process_control_block {
	size_t pid;
	size_t ppid;
	int exit_status;
	bool kill_signal;

	enum process_state process_state;
	struct pm_wait_queue* parent_wait_queue;

	void* pagemap_hhdm_ptr;
	void* process_heap_ptr;

	struct interrupt_info* interrupt_info;

	struct vfs_file* fds[MAX_PROCESS_FDS];

	struct process_control_block* next;
};

void init_process_manager();
int64_t pm_execve(struct process_control_block* pcb, const char* path,
				  struct interrupt_info* process_regs);
int pm_fork(struct process_control_block* pcb, struct interrupt_info* process_regs);
void pm_exit(int status, struct process_control_block* pcb, struct interrupt_info* process_regs);
/** Waits for a terminated child
 * @return the pid of the process that is returned */
size_t pm_wait(int* status, struct process_control_block* pcb, struct interrupt_info* process_regs);
size_t pm_kill(size_t pid, struct process_control_block* pcb, struct interrupt_info* process_regs);

struct process_control_block* pm_get_curr_pcb();
/** @return PCB of requested pid, NULL if there is no process with that pid */
struct process_control_block* pm_get_pcb_by_pid(size_t pid);

struct pm_wait_queue* pm_create_wait_queue(void);
/** Changes the current process sate to waiting and adds it to the device_wait_queue */
void pm_waitqueue_enqueue(struct pm_wait_queue* wait_queue);
/** Marks all the processes in the wait queue as ready again */
void pm_waitqueue_dequeue_all(struct pm_wait_queue* wait_queue);
/** Loads a process ready to run.
 * If there are no ready processes waits for a ready one and uses it.
 * @return The ready process CPU state */
struct interrupt_info* pm_load_next_ready_process();
