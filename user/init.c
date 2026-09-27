typedef unsigned long long uint64_t;
typedef long long int64_t;
typedef long long ssize_t;

static int64_t syscall(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
	int64_t ret;

	asm volatile("syscall"
				 : "=a"(ret)
				 : "a"(syscall_num), "D"(arg1), "S"(arg2), "d"(arg3)
				 : "memory");

	return ret;
}

ssize_t read(int64_t file_desc, void* buf, uint64_t count) {
	return syscall(0, file_desc, (uint64_t)buf, count);
}
ssize_t write(int64_t file_desc, void* buf, uint64_t count) {
	return syscall(1, file_desc, (uint64_t)buf, count);
}
ssize_t open(const char* path, uint64_t flags) { return syscall(2, (uint64_t)path, flags, 0); }
ssize_t close(int64_t file_desc) { return syscall(3, file_desc, 0, 0); }
int64_t dup(int64_t file_desc) { return syscall(32, file_desc, 0, 0); }
int64_t dup2(int64_t newfd, int64_t oldfd) { return syscall(33, newfd, oldfd, 0); }

#define BUFFER_LEN 10
void _start() {
	char buf[BUFFER_LEN];

	while (1) {
		ssize_t ret = read(0, buf, 1);
		if (ret == 0) {
			continue;
		}
		write(1, buf, 1);
	}

	while (1) {
	}
}
