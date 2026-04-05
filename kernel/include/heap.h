#pragma once
#include <include/types.h>
#include <include/vendor/limine.h>

void init_kernel_heap(void);

void* kmalloc(size_t size);
void kfree(void* addr);
