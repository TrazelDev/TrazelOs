#pragma once
#include <include/types.h>

__attribute__((noreturn)) void kernel_panic(const char* msg, const char* file_name, size_t line);

#define KERNEL_PANIC(msg) kernel_panic(msg, __FILE__, __LINE__)
#define KERNEL_ASSERT(condition)                                                \
	if (!(condition)) {                                                         \
		kernel_panic("Assertion failed  (" #condition ")", __FILE__, __LINE__); \
	}
