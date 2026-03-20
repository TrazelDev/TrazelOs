#pragma once
#include "utility/utility.h"



void initMemoryPool();
void* malloc_x(size_t blockSize);
void free_x(void* blockPtr, size_t blockSize);
