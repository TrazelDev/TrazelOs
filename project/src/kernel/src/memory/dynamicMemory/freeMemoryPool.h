#pragma once
#include "utility/utility.h"



void initMemoryPool();
void* malloc(size_t blockSize);
void free(void* blockPtr, size_t blockSize);
