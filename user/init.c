typedef unsigned long long uint64_t;
typedef long long int64_t;

static int64_t syscall(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
	int64_t ret;

	asm volatile("syscall"
				 : "=a"(ret)
				 : "a"(syscall_num), "D"(arg1), "S"(arg2), "d"(arg3)
				 : "memory");

	return ret;
}

void _start() {
	char buf[2];
	buf[1] = '\0';

	while (1) {
		syscall(0, 0, (uint64_t)buf, 1);
		syscall(1, 1, (uint64_t)buf, 1);
	}

	while (1) {
	}
}
