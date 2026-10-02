#include <stdint.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>

#undef errno
extern int errno;

char** environ;

static int64_t syscall(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
	int64_t ret;
	asm volatile("syscall"
				 : "=a"(ret)
				 : "a"(syscall_num), "D"(arg1), "S"(arg2), "d"(arg3)
				 : "rcx", "r11", "memory");
	return ret;
}

void _exit(int status) {
	syscall(60, (uint64_t)status, 0, 0);
	while (1);
}

int read(int file, char* ptr, int len) {
	return (int)syscall(0, (uint64_t)file, (uint64_t)ptr, (uint64_t)len);
}

int write(int file, char* ptr, int len) {
	return (int)syscall(1, (uint64_t)file, (uint64_t)ptr, (uint64_t)len);
}

int open(const char* name, int flags, ...) {
	return (int)syscall(2, (uint64_t)name, (uint64_t)flags, 0);
}

int close(int file) { return (int)syscall(3, (uint64_t)file, 0, 0); }

int execve(char* name, char** argv, char** env) {
	return (int)syscall(59, (uint64_t)name, (uint64_t)argv, (uint64_t)env);
}

int fork() { return (int)syscall(57, 0, 0, 0); }

int getpid() { return (int)syscall(39, 0, 0, 0); }

int wait(int* status) { return (int)syscall(61, (uint64_t)status, 0, 0); }

int kill(int pid, int sig) { return (int)syscall(62, (uint64_t)pid, (uint64_t)sig, 0); }

caddr_t sbrk(int incr) {
	uint64_t current_brk = syscall(12, 0, 0, 0);

	if (incr == 0) {
		return (caddr_t)current_brk;
	}

	uint64_t new_brk = syscall(12, current_brk + incr, 0, 0);

	if (new_brk != current_brk + incr) {
		errno = ENOMEM;
		return (caddr_t)-1;
	}

	return (caddr_t)current_brk;
}

int fstat(int file, struct stat* st) {
	errno = ENOSYS;
	return -1;
}

int isatty(int file) {
	errno = ENOSYS;
	return 0;
}

int link(char* old, char* new) {
	errno = ENOSYS;
	return -1;
}

int lseek(int file, int ptr, int dir) {
	return syscall(8, (uint64_t)file, (uint64_t)ptr, (uint64_t)dir);
}

int stat(const char* file, struct stat* st) {
	errno = ENOSYS;
	return -1;
}

clock_t times(struct tms* buf) {
	errno = ENOSYS;
	return -1;
}

int unlink(char* name) {
	errno = ENOSYS;
	return -1;
}

int gettimeofday(struct timeval* p, void* z) {
	errno = ENOSYS;
	return -1;
}
