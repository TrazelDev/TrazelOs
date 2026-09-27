#include <include/types.h>
#include <kernel/include/vfs.h>

#include "kernel/include/process_manager.h"
#include "syscall_fileops.h"

static bool is_fd_valid(int64_t file_desc, struct vfs_file** fd_table);

void syscall_write_handler(struct process_control_block* pcb, struct interrupt_info* process_regs) {
	int64_t file_desc = (int64_t)process_regs->rdi;
	if (!is_fd_valid(file_desc, pcb->fds)) {
		process_regs->rax = -1;
		return;
	}

	struct vfs_file* file = pcb->fds[file_desc];
	uint8_t* output_buf = (uint8_t*)process_regs->rsi;
	size_t buf_len = process_regs->rdx;

	int64_t bytes_wrote = vfs_write(file, output_buf, buf_len);
	process_regs->rax = bytes_wrote;
}

void syscall_read_handler(struct process_control_block* pcb, struct interrupt_info* process_regs) {
	int64_t file_desc = (int64_t)process_regs->rdi;
	if (!is_fd_valid(file_desc, pcb->fds)) {
		process_regs->rax = -1;
		return;
	}

	struct vfs_file* file = pcb->fds[file_desc];
	uint8_t* buf = (uint8_t*)process_regs->rsi;
	size_t buf_len = process_regs->rdx;

	int64_t bytes_read = vfs_read(file, buf, buf_len);
	process_regs->rax = bytes_read;
}

void syscall_open_handler(struct process_control_block* pcb, struct interrupt_info* process_regs) {
	struct vfs_file* file = vfs_open((char*)process_regs->rdi);
	if (file == NULL) {
		process_regs->rax = -1;
		return;
	}

	for (uint64_t i = 0; i < MAX_PROCESS_FDS; i++) {
		if (pcb->fds[i] != NULL) {
			continue;
		}

		pcb->fds[i] = file;
		process_regs->rax = i;
		return;
	}

	process_regs->rax = -1;
}

void syscall_close_handler(struct process_control_block* pcb, struct interrupt_info* process_regs) {
	int64_t file_desc = (int64_t)process_regs->rdi;
	if (!is_fd_valid(file_desc, pcb->fds)) {
		process_regs->rax = -1;
		return;
	}

	struct vfs_file* file = pcb->fds[file_desc];
	process_regs->rax = vfs_close(file);
	pcb->fds[file_desc] = NULL;
}

void syscall_dup_handler(struct process_control_block* pcb, struct interrupt_info* process_regs) {
	int64_t file_desc = (int64_t)process_regs->rdi;
	if (!is_fd_valid(file_desc, pcb->fds)) {
		process_regs->rax = -1;
		return;
	}

	struct vfs_file* file = pcb->fds[file_desc];
	for (uint64_t i = 0; i < MAX_PROCESS_FDS; i++) {
		if (pcb->fds[i] != NULL) {
			continue;
		}

		file->file_ref_count++;
		pcb->fds[i] = file;
		process_regs->rax = i;
		return;
	}

	process_regs->rax = -1;
}

void syscall_dup2_handler(struct process_control_block* pcb, struct interrupt_info* process_regs) {
	int64_t oldfd = (int64_t)process_regs->rdi;
	int64_t newfd = (int64_t)process_regs->rsi;

	if (!is_fd_valid(oldfd, pcb->fds) || newfd < 0 || newfd >= MAX_PROCESS_FDS) {
		process_regs->rax = -1;
		return;
	}

	if (oldfd == newfd) {
		process_regs->rax = newfd;
		return;
	}

	if (is_fd_valid(newfd, pcb->fds)) {
		vfs_close(pcb->fds[newfd]);
	}

	pcb->fds[oldfd]->file_ref_count++;
	pcb->fds[newfd] = pcb->fds[oldfd];
	process_regs->rax = newfd;
}

// module private functions:
// -------------------------------------------------------------------------------------------------

static bool is_fd_valid(int64_t file_desc, struct vfs_file** fd_table) {
	if (file_desc < 0 || file_desc >= MAX_PROCESS_FDS) {
		return false;
	}

	struct vfs_file* file = fd_table[file_desc];
	if (file == NULL) {
		return false;
	}

	return true;
}
